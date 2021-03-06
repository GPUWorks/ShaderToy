#include "common.h"
#include "shader.h"
#include "filesystem.h"

static const char *types[] = {
    "Vertex",
    "Fragment",
    ""
};

Shader::Shader(Shader::ShaderType shaderType)
    : mShaderId(0),
      mShaderType(shaderType)
{
}

Shader::~Shader()
{
}

bool Shader::compileSourceCode(const char *source)
{
    return compile(source);
}

bool Shader::compileSourceCode(const std::string &source)
{
    return compile(source.c_str());
}

bool Shader::compileSourceFile(const std::string &filename)
{
    bool ret = true;
    char *source = FileSystem::readAll(filename.c_str());
    if (source == nullptr)
        return false;

    ret = compile(source);
    delete[] source;
    return ret;
}

bool Shader::create()
{
    if (mShaderType == Shader::ShaderType::Vertex)
        mShaderId = glCreateShader(GL_VERTEX_SHADER);
    else if (mShaderType == Shader::ShaderType::Fragment)
        mShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    if (!mShaderId)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Could not create shader of type: %d.\n", (int)mShaderType);
        return false;
    }
    else
        return true;
}

void Shader::destroy()
{
    if (!mShaderId)
        return;

    glDeleteShader(mShaderId);
    mShaderId = 0;
}

bool Shader::compile(const char *source)
{
    if (!create())
        return false;

    glShaderSource(mShaderId, 1, &source, nullptr);
    glCompileShader(mShaderId);

    int value;
    glGetShaderiv(mShaderId, GL_COMPILE_STATUS, &value);
    mCompiled = (value != 0);

    int sourceCodeLength = 0;
    glGetShaderiv(mShaderId, GL_SHADER_SOURCE_LENGTH, &sourceCodeLength);
    if (sourceCodeLength > 1)
    {
        int temp = 0;
        mSourceCode.resize(sourceCodeLength);
        glGetShaderSource(mShaderId, sourceCodeLength, &temp, &mSourceCode[0]);
    }

    if (!mCompiled)
    {
        glGetShaderiv(mShaderId, GL_INFO_LOG_LENGTH, &value);

        if (value > 1)
        {
            int length;
            mLog.resize(value);
            glGetShaderInfoLog(mShaderId, value, &length, &mLog[0]);

            const char *type = types[2];
            if (mShaderType == Shader::ShaderType::Vertex)
                type = types[0];
            else if (mShaderType == Shader::ShaderType::Fragment)
                type = types[1];

            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Shader::compile(%s): %s", type, mLog.c_str());
        }
    }

    return mCompiled;
}
