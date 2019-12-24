
const { gl, vao, instanced } = require("./WebGLContext");
// const Shader = require("./Shader");

const PrimitiveType = {
    eLines     = 1,
    eTriangles = 2,
}

const AttributeType = {
    eFloat = 1,
    eVec2f = 2,
    eVec3f = 3,
    eVec4f = 4,
    eMat3f = 5,
    eMat4f = 6
}

// interface IVboAttrDef {
//     name: string;
//     type: AttributeType;
//     index: number;
// }

// interface IVboDef {
//     stride: number;
//     instanced: boolean;
//     attributes: IVboAttrDef[];
// };

// interface IGeometryDef {
//     shader: Shader
//     primitiveType: PrimitiveType;
//     vbos: IVboDef[];
// }

// class Primitive {
//     type: number;
//     index: number;
//     count: number;
// }

class Geometry {

    // private _primitive: Primitive;
    // // private _primitive.type: number;
    // // private _primitive.index: number;
    // // private _primitive.count: number;
    // private _isInstanced: boolean;
    // private _instanceCount: number;
    // private _vbos: WebGLBuffer[];
    // private _vao: WebGLVertexArrayObjectOES;

    constructor(def /*: IGeometryDef*/) {

        this._primitive = {
            type: gl.LINES,
            index: 0,
            count: 0,
        };
    
        switch (def.primitiveType) {
            // case PrimitiveType.eLines:
            //     this._primitive.type = gl.LINES;
            //     break;
            case PrimitiveType.eTriangles:
                this._primitive.type = gl.TRIANGLES;
                break;
        }

        this._isInstanced = false;
        this._instanceCount = 0;

        this._vbos = [];

        this._vao = vao.createVertexArrayOES();
        vao.bindVertexArrayOES(this._vao);

        const bpp = 4; // 4 bytes (gl.FLOAT)

        def.vbos.forEach((vbo) => {

            let stride = vbo.stride * bpp;

            const newVbo = gl.createBuffer();

            gl.bindBuffer(gl.ARRAY_BUFFER, newVbo);

            vbo.attributes.forEach((attribute) => {

                let rowSize = 1;
                let totalRows = 1;
                switch (attribute.type) {
                    case AttributeType.eFloat:
                        rowSize = 1;
                        break;
                    case AttributeType.eVec2f:
                        rowSize = 2;
                        break;
                    case AttributeType.eVec3f:
                        rowSize = 3;
                        break;
                    case AttributeType.eVec4f:
                        rowSize = 4;
                        break;
                    case AttributeType.eMat3f:
                        rowSize = 3;
                        totalRows = 3;
                        break;
                    case AttributeType.eMat4f:
                        rowSize = 4;
                        totalRows = 4;
                        break;
                }

                const attrLocation = def.shader.getAttributes(attribute.name);

                for (let ii = 0; ii < totalRows; ++ii) {

                    const attrId = attrLocation + ii;
                    const rowIndex = (attribute.index + ii * rowSize) * bpp;

                    gl.enableVertexAttribArray(attrId);
                    gl.vertexAttribPointer(attrId, rowSize, gl.FLOAT, false, stride, rowIndex);

                    if (vbo.instanced) {
                        instanced.vertexAttribDivisorANGLE(attrId, 1);
                        this._isInstanced = true;
                    }
                }
            });

            this._vbos.push(newVbo);
        });

        vao.bindVertexArrayOES(null);
    }

    dispose() {
        this._vbos.forEach(vbo => gl.deleteBuffer(vbo));
        vao.deleteVertexArrayOES(this._vao);
    }

    update(index, vertices, dynamic) {

        if (index < 0 || index >= this._vbos.length)
            throw new Error("update, out of range index");

        gl.bindBuffer(gl.ARRAY_BUFFER, this._vbos[index]);

        const usage = (dynamic ? gl.DYNAMIC_DRAW : gl.STATIC_DRAW);
        gl.bufferData(gl.ARRAY_BUFFER, vertices, usage);
    }

    setPrimitiveRange(index, count) {

        if (index < 0)
            throw new Error("setPrimitiveRange, index cannot be negative");
        if (count < 0)
            throw new Error("setPrimitiveRange, count cannot be negative");

        this._primitive.index = index;
        this._primitive.count = count;
    }

    setInstancedCount(count) {
        this._instanceCount = count;
    }

    render() {

        if (this._primitive.count == 0 || (this._isInstanced && this._instanceCount == 0))
            return;

        vao.bindVertexArrayOES(this._vao);

        if (this._isInstanced) {
            instanced.drawArraysInstancedANGLE(
                this._primitive.type,
                this._primitive.index,
                this._primitive.count,
                this._instanceCount
            );
        }
        else {
            gl.drawArrays(
                this._primitive.type,
                this._primitive.index,
                this._primitive.count
            );
        }

        vao.bindVertexArrayOES(null);
    }
}

export { Geometry, PrimitiveType, AttributeType };
