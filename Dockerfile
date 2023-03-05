# syntax=docker/dockerfile:1

#########################
# To build docker image:
#########################

# docker build -t minimal_get_opengl_version --network=host --progress=plain .


###########################
# To copy build executable:
###########################

# id=$(docker create minimal_get_opengl_version) && docker cp $id:/MinimalGetOpenGLVersion/_build/Release/GetOpenGLVersion GetOpenGLVersion && docker rm -v $id

FROM quay.io/pypa/manylinux2010_x86_64:latest
WORKDIR /MinimalGetOpenGLVersion
COPY . .
RUN ./build.sh