
function createRectangleVertices(sizeX, sizeY, color) {
    return [
        +sizeX, +sizeY,   color[0],color[1],color[2], // top right
        -sizeX, +sizeY,   color[0],color[1],color[2], // top left
        +sizeX, -sizeY,   color[0],color[1],color[2], // bottom right
        +sizeX, -sizeY,   color[0],color[1],color[2], // bottom right
        -sizeX, +sizeY,   color[0],color[1],color[2], // top left
        -sizeX, -sizeY,   color[0],color[1],color[2], // bottom left
    ];
}

function createCircleVertices(radius, quality, color) {

    const vertices = [];

    vertices.push(0, 0);
    for (let ii = 0; ii < quality; ++ii) {

        const ratio = ii / quality;

        vertices.push(radius * Math.cos(Math.PI * 2 * ratio));
        vertices.push(radius * Math.sin(Math.PI * 2 * ratio));
    }

    //

    const indices = [];

    for (let ii = 1; ii < quality + 1; ++ii)
        indices.push(0, ii, (ii + 1));
    indices.push(0, quality, 1);

    //

    const fvertices = [];

    for (let ii = 0; ii < indices.length; ++ii) {

        const currIndex = indices[ii] * 2;

        fvertices.push( vertices[currIndex + 0] );
        fvertices.push( vertices[currIndex + 1] );

        fvertices.push( color[0] );
        fvertices.push( color[1] );
        fvertices.push( color[2] );
    }

    return fvertices;
}

module.exports = { createRectangleVertices, createCircleVertices };
