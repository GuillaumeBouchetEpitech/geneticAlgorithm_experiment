


rm -rf ./archive
mkdir -p ./archive

rsync -r \
    . \
    --exclude "bullet3" \
    --exclude "glm" \
    --exclude "tinyobjloader" \
    --exclude "tools" \
    --exclude "obj" \
    --exclude "lib" \
    --exclude "bin" \
    --exclude "archive" \
    ./archive/

zip

