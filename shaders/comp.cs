/*#version 430 core

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

// ----------------------------------------------------------------------------
//
// uniforms
//
// ----------------------------------------------------------------------------

layout(rgba32f, binding = 0) uniform image2D tex;

uniform float time;                

// ----------------------------------------------------------------------------
//
// functions
//
// ----------------------------------------------------------------------------

void main() {
    vec4 value = vec4(0.0, 0.0, 0.0, 1.0);
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
    float speed = 5.0; // Ajout du point décimal pour la clarté
    float width = 512.0; // Ajout du point décimal pour la clarté

    // Utilisez la variable time pour modifier la coordonnée y
    float y = sin(0.1*texelCoord.x + time * speed); //sinusoide

    value.x = y;
    value.y = y;
    value.z = y;

    imageStore(tex, texelCoord, value);
}*/

#version 430 core

layout(local_size_x = 16, local_size_y = 16) in;

// Tailles constantes pour les tableaux
const int MAX_SIZE = 256; // Taille maximale du tableau, ajustez selon vos besoins

// Textures pour la IFFT et la Butterfly Texture
layout(binding = 0, rgba32f) readonly uniform image2D inputTexture; // Pour lire les données d'entrée
layout(binding = 1, rgba32f) writeonly uniform image2D outputTexture; // Pour écrire les résultats de la IFFT
layout(binding = 2, rgba32f) writeonly uniform image2D butterflyTexture; // Pour stocker les données de la Butterfly Texture

uniform int N; // Taille de la FFT
uniform int numStages; // Nombre d'étapes de la FFT

// Structure pour les nombres complexes
struct Complex {
    float real;
    float imaginary;
};

// Multiplication complexe
Complex complexMul(Complex a, Complex b) {
    Complex result;
    result.real = a.real * b.real - a.imaginary * b.imaginary;
    result.imaginary = a.real * b.imaginary + a.imaginary * b.real;
    return result;
}

// Addition complexe
Complex complexAdd(Complex a, Complex b) {
    Complex result;
    result.real = a.real + b.real;
    result.imaginary = a.imaginary + b.imaginary;
    return result;
}

// Génère la Butterfly Texture et stocke les données
void generateButterflyTexture(Complex twiddleFactors[MAX_SIZE], int indicesA[MAX_SIZE], int indicesB[MAX_SIZE]) {
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);

    int stage = coord.x; // L'étape actuelle
    int element = coord.y; // L'élément dans l'étape

    // Calcule le facteur de correction (twiddle factor)
    float angle = -2.0 * 3.14159265359 * (float(element) / pow(2, stage + 1));
    Complex twiddle = Complex(cos(angle), sin(angle));

    // Calcule les indices des entrées de papillon
    int span = int(pow(2, stage));
    int indexA = element;
    int indexB = element + span;

    // Stocke les informations de papillon dans les tableaux locaux
    twiddleFactors[coord.y + stage * 16] = twiddle;
    indicesA[coord.y + stage * 16] = indexA;
    indicesB[coord.y + stage * 16] = indexB;

    // Écriture des données dans la Butterfly Texture
    imageStore(butterflyTexture, coord, vec4(twiddle.real, twiddle.imaginary, float(indexA), float(indexB)));
}

// Effectue les opérations de la IFFT avec les données de la Butterfly Texture
void performIFFT(Complex twiddleFactors[MAX_SIZE], int indicesA[MAX_SIZE], int indicesB[MAX_SIZE]) {
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);

    int stage = coord.x; // L'étape actuelle

    // Récupère les informations des tableaux locaux
    Complex twiddle = twiddleFactors[coord.y + stage * 16];
    int indexA = indicesA[coord.y + stage * 16];
    int indexB = indicesB[coord.y + stage * 16];

    // Lecture des données complexes depuis inputTexture
    vec2 dataA = imageLoad(inputTexture, ivec2(coord.x, coord.y)).rg; // Lecture d'un vec2
    Complex a = Complex(dataA.x, dataA.y); // Initialisation explicite
    
    vec2 dataB = imageLoad(inputTexture, ivec2(coord.x, coord.y)).rg; // Lecture d'un vec2
    Complex b = Complex(dataB.x, dataB.y); // Initialisation explicite

    // Effectue les opérations de la IFFT
    Complex resultA = complexAdd(a, complexMul(twiddle, b));
    Complex resultB = complexAdd(a, complexMul(twiddle, b));

    // Stocke les résultats de la IFFT dans outputTexture
    imageStore(outputTexture, ivec2(indexA, coord.y), vec4(resultA.real, resultA.imaginary, 0.0, 1.0));
    imageStore(outputTexture, ivec2(indexB, coord.y), vec4(resultB.real, resultB.imaginary, 0.0, 1.0));
}

void main() {
    // Création des tableaux temporaires pour stocker les données de la Butterfly Texture
    Complex twiddleFactors[MAX_SIZE]; // Stockage des twiddle factors
    int indicesA[MAX_SIZE]; // Indices des premières entrées de papillon
    int indicesB[MAX_SIZE]; // Indices des secondes entrées de papillon

    // Génère la Butterfly Texture
    generateButterflyTexture(twiddleFactors, indicesA, indicesB);

    // Exécute les opérations de la IFFT
    performIFFT(twiddleFactors, indicesA, indicesB);
}
