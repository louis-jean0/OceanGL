#include <glad/glad.h>
#include <Plane.hpp>


Plane::Plane(float size, int div) {
    this->size = size;
    this->div = div;
    this->ex = this->div;
    this->texIter = this->div;

    std::string textures[6] = {
        "../textures/Skybox/px.png",
        "../textures/Skybox/nx.png",
        "../textures/Skybox/py.png",
        "../textures/Skybox/ny.png",
        "../textures/Skybox/pz.png",
        "../textures/Skybox/nz.png"
    };
    std::copy(std::begin(textures), std::end(textures), std::begin(this->pathTextures));
}

void Plane::createPlane() {
    this->div = div;
    this->va.delVAO();
    this->vb.delVBO();
    this->eb.delEBO();
    delArrays();

    int numVertices = (div + 1) * (div + 1);
    float scale = size / div;

    for (int h = 0; h < this->div + 1; h++) {
        for (int w = 0; w < this->div + 1; w++) {
            float x = (float)w * scale - size / 2.0f;
            float y = 0.0f;
            float z = (float)h * scale - size / 2.0f; 

            this->verts.push_back(x);
            this->verts.push_back(y);
            this->verts.push_back(z);
            
            this->verts.push_back((float)w / div);
            this->verts.push_back((float)h / div);
 

            glm::vec3 normal(0., -1., 0.);
            glm::vec3 tangente(1., 0., 0.); 
            glm::vec3 bitangente(0., 0., 1.); 

            this->verts.push_back(normal.x);
            this->verts.push_back(normal.y);
            this->verts.push_back(normal.z);
        }
    }

    for (int h = 0; h < div; h++) {
        for (int w = 0; w < div; w++) {
            int index = h * (div + 1) + w;
            indices.push_back(index);
            indices.push_back(index + 1);
            indices.push_back(index + div + 1);
            indices.push_back(index + 1);
            indices.push_back(index + div + 2);
            indices.push_back(index + div + 1);
        }
    }

    vb.genVBO(verts);
    va.genVAO();
    eb.genEBO(indices);
}

void Plane::subdivisediv(int div) {
    this->div = div;

    this->va.delVAO();
    this->vb.delVBO();
    this->eb.delEBO();
    delArrays();

    int numVertices = (div + 1) * (div + 1);
    float scale = size / div;

    for (int h = 0; h < this->div + 1; h++) {
        for (int w = 0; w < this->div + 1; w++) {
            float x = (float)w * scale - size / 2.0f;
            float y = 0.0f;
            float z = (float)h * scale - size / 2.0f; 

            this->verts.push_back(x);
            this->verts.push_back(y);
            this->verts.push_back(z);
            
            this->verts.push_back((float)w / div);
            this->verts.push_back((float)h / div);


            glm::vec3 normal(0., 1., 0.); 


            this->verts.push_back(normal.x);
            this->verts.push_back(normal.y);
            this->verts.push_back(normal.z);
        }
    }

    for (int h = 0; h < div; h++) {
        for (int w = 0; w < div; w++) {
            int index = h * (div + 1) + w;
            indices.push_back(index);
            indices.push_back(index + 1);
            indices.push_back(index + div + 1);
            indices.push_back(index + 1);
            indices.push_back(index + div + 2);
            indices.push_back(index + div + 1);
        }
    }

    vb.genVBO(verts);
    va.genVAO();
    eb.genEBO(indices);
}

void Plane::updateSize(float nouvelleTaille) {
    this->size = nouvelleTaille;
    this->subdivisediv(this->div);
}

// Pour le moment, seul les GL_TRIANGLES sont pris en compte
void Plane::updatePlane(GLenum mode) {
    glBindVertexArray(this->va.getVAO()); 
    glDrawElements(mode, this->indices.size(), GL_UNSIGNED_INT, 0); 
}

void Plane::delPlane() {
    delArrays();
    this->plane.deleteShader();
    this->va.delVAO();
    this->vb.delVBO();
    this->eb.delEBO();
}

void Plane::delArrays() {
    this->verts.clear();
    this->indices.clear();
}

// Shader
void Plane::useShader() {
    this->plane.useShader();
}

void Plane::useComputeShader() {
    this->planeComp.useComputeShader();
}


void Plane::attachShader(const GLchar* vertexPath, const GLchar* fragmentPath) {
    this->plane.setShader(vertexPath, fragmentPath);
}

void Plane::attachShaderComp(const GLchar* compPath) {
    this->planeComp.setComputeShader(compPath);
}

void Plane::detachShader() {
    this->plane.Program = NULL;
}

void Plane::Shaderbind1f(const GLchar* name, GLfloat v0) {
    this->plane.setBind1f(name, v0);
}

void Plane::Shaderbind3f(const GLchar* name, GLfloat v0, GLfloat v1, GLfloat v2) {
    this->plane.setBind3f(name, v0, v1, v2);
}

void Plane::ShaderbindMatrix4fv(const GLchar* name, const GLfloat *value) {
    this->plane.setBindMatrix4fv(name, 1, GL_FALSE, value);
}

void Plane::ShaderbindMatrix3fv(const GLchar* name, const GLfloat *value) {
    this->plane.setBindMatrix3fv(name, 1, GL_FALSE, value);
}

void Plane::Shaderbind1i(const GLchar* name, GLint v0) {
    this->plane.setBind1i(name, v0);
}

Shader Plane::getShader() {
    return this->plane;
}

ComputeShader Plane::getShaderComp() {
    return this->planeComp;
}

void Plane::ComputeWorkGroup() {
    int max_compute_work_group_count[3];
	int max_compute_work_group_size[3];
	int max_compute_work_group_invocations;

	for (int idx = 0; idx < 3; idx++) {
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, idx, &max_compute_work_group_count[idx]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, idx, &max_compute_work_group_size[idx]);
	}	
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &max_compute_work_group_invocations);    
}

void Plane::DispatchWorkGroup(int width, int height, int wkw, int wkh) {
	glDispatchCompute((unsigned int)width/wkw, (unsigned int)height/wkh, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT); 
}

void Plane::loadCubemap(){
    glGenTextures(1, &(this->textureID));
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < 6; i++)
    {
        unsigned char *data = stbi_load(this->pathTextures[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            //std::cout << "Text " << i << " : width = " << width << ", height = " << height << ", nr = " << nrChannels << "\n";
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << this->pathTextures[i].c_str() << std::endl;
            stbi_image_free(data);
            return;
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Axe x
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Axe y
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); // Axe z
}

void Plane::bindCubemap(GLenum TextureUnit, int unit){
    glActiveTexture(TextureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->textureID);
    glUniform1i(glGetUniformLocation(this->plane.Program, "skyboxTexture"), unit);
}
