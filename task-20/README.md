# CG-lab2

### void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat \*value);
Specify the value of a uniform variable for the current program object.
  - *location*: Specifies the location of the uniform variable to be modified.
  - *count*: For the matrix (glUniformMatrix*) commands, specifies the number of matrices that are to be modified. This should be 1 if the targeted uniform variable is not an array of matrices, and 1 or more if it is an array of matrices.
  - *transpose*: For the matrix commands, specifies whether to transpose the matrix as the values are loaded into the uniform variable.
  - *value*: For the vector and matrix commands, specifies a pointer to an array of count values that will be used to update the specified uniform variable.


### GLuint glCreateProgram();
Creates a program object


### GLuint glCreateShader( GLenum shaderType);
Creates a shader object
  - *shaderType*: Specifies the type of shader to be created. Must be one of GL_COMPUTE_SHADER, GL_VERTEX_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER, or GL_FRAGMENT_SHADER.


### void glShaderSource( GLuint shader, GLsizei count, const GLchar **string, const GLint \*length);
Replaces the source code in a shader object
  - *shader*: Specifies the handle of the shader object whose source code is to be replaced.
  - *count*: Specifies the number of elements in the string and length arrays.
  - *string*: Specifies an array of pointers to strings containing the source code to be loaded into the shader.
  - *length*: Specifies an array of string lengths.


### void glAttachShader(	GLuint program, GLuint shader);
Attaches a shader object to a program object
  - *program*: Specifies the program object to which a shader object will be attached.
  - *shader*: Specifies the shader object that is to be attached.


### glValidateProgram(program);
Validates a program object
  - *program*: Specifies the handle of the program object to be validated.


### void glLinkProgram( GLuint program);
Links a program object
  - *program*: Specifies the handle of the program object to be linked.


### void glGetProgramiv(	GLuint program, GLenum pname, GLint \*params);
Return a parameter from a program object
  - *program*: Specifies the program object to be queried.
  - *pname*: Specifies the object parameter. Accepted symbolic names are GL_DELETE_STATUS, GL_LINK_STATUS, GL_VALIDATE_STATUS, GL_INFO_LOG_LENGTH, GL_ATTACHED_SHADERS, GL_ACTIVE_ATTRIBUTES, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, GL_ACTIVE_UNIFORMS, GL_ACTIVE_UNIFORM_MAX_LENGTH.
  - *params*: Returns the requested object parameter.


### void glGetShaderiv( GLuint shader, GLenum pname, GLint \*params);
Return a parameter from a shader object
  - *shader*: Specifies the shader object to be queried.
  - *pname*: Specifies the object parameter. Accepted symbolic names are GL_SHADER_TYPE, GL_DELETE_STATUS, GL_COMPILE_STATUS, GL_INFO_LOG_LENGTH, GL_SHADER_SOURCE_LENGTH.
  - *params*: Returns the requested object parameter.


### void glCompileShader( GLuint shader);
Compiles a shader object
  - *shader*: Specifies the shader object to be compiled.


### GLint glGetUniformLocation( GLuint program, const GLchar \*name);
Returns the location of a uniform variable
  - *program*: Specifies the program object to be queried.
  - *name*: Points to a null terminated string containing the name of the uniform variable whose location is to be queried.


### void glUseProgram(	GLuint program);
Installs a program object as part of current rendering state
  - *program*: Specifies the handle of the program object whose executables are to be used as part of current rendering state.


