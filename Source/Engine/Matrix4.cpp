//
//  Matrix4.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 21.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "Matrix4.h"

Matrix4::Matrix4() {
    
}

Matrix4::Matrix4(Matrix4 const &mat) {
    data.x = mat.data.x;
    data.y = mat.data.y;
    data.z = mat.data.z;
    data.w = mat.data.w;
}

Matrix4::Matrix4(btTransform const &mat) {
    btMatrix3x3 basis = mat.getBasis();
    data.x = basis.getColumn(0);
    data.y = basis.getColumn(1);
    data.z = basis.getColumn(2);
    data.w = mat.getOrigin();
    data.w.setW(1.0);
}

Matrix4::Matrix4(btScalar matData[16]) {
    memcpy(values, matData, sizeof(values));
}

std::string Matrix4::getString() {
    char buffer[64];
    std::string str = "(";
    for(unsigned char i = 0; i < 16; i ++) {
        if(i % 4 > 0) str += ", ";
        sprintf(buffer, "%f", values[i]);
        str += buffer;
        if(i % 4 == 3 && i < 15) str += ",\n ";
    }
    return str+")";
}

btMatrix3x3 Matrix4::getNormalMatrix() {
    return btMatrix3x3(data.x.normalized(), data.y.normalized(), data.z.normalized());
}

btTransform Matrix4::getBTMatrix() {
    btTransform mat;
    mat.setBasis(btMatrix3x3(data.x, data.y, data.z).transpose());
    mat.setOrigin(data.w);
    return mat;
}

void Matrix4::getOpenGLMatrix(btScalar* matData) const {
    matData[0] = data.x.x();
    matData[1] = data.y.x();
    matData[2] = data.z.x();
    matData[3] = data.w.x();
    matData[4] = data.x.y();
    matData[5] = data.y.y();
    matData[6] = data.z.y();
    matData[7] = data.w.y();
    matData[8] = data.x.z();
    matData[9] = data.y.z();
    matData[10] = data.z.z();
    matData[11] = data.w.z();
    matData[12] = data.x.w();
    matData[13] = data.y.w();
    matData[14] = data.z.w();
    matData[15] = data.w.w();
    //Matrix4 a = getTransposed();
    //memcpy(matData, a.values, sizeof(values));
}

Matrix4& Matrix4::setIdentity() {
    data.x = btVector3(1, 0, 0);
    data.y = btVector3(0, 1, 0);
    data.z = btVector3(0, 0, 1);
    data.w = btVector3(0, 0, 0);
    data.w.setW(1);
    return *this;
}

Matrix4 Matrix4::getTransposed() const {
    Matrix4 a;
    a.data.x.setX(data.x.x());
    a.data.x.setY(data.y.x());
    a.data.x.setZ(data.z.x());
    a.data.x.setW(data.w.x());
    a.data.y.setX(data.x.y());
    a.data.y.setY(data.y.y());
    a.data.y.setZ(data.z.y());
    a.data.y.setW(data.w.y());
    a.data.z.setX(data.x.z());
    a.data.z.setY(data.y.z());
    a.data.z.setZ(data.z.z());
    a.data.z.setW(data.w.z());
    a.data.w.setX(data.x.w());
    a.data.w.setY(data.y.w());
    a.data.w.setZ(data.z.w());
    a.data.w.setW(data.w.w());
    return a;
}

Matrix4 Matrix4::getInverse() {
    //ftp://download.intel.com/design/PentiumIII/sml/24504301.pdf
#if defined (BT_USE_SSE)
    Matrix4 dst = *this;
    __m128 minor0, minor1, minor2, minor3, row0, row1, row2, row3, det, tmp1;
    tmp1 = _mm_loadh_pi(_mm_loadl_pi(tmp1, (__m64*)(dst.values)), (__m64*)(dst.values+4));
    row1 = _mm_loadh_pi(_mm_loadl_pi(row1, (__m64*)(dst.values+8)), (__m64*)(dst.values+12));
    row0 = _mm_shuffle_ps(tmp1, row1, 0x88);
    row1 = _mm_shuffle_ps(row1, tmp1, 0xDD);
    tmp1 = _mm_loadh_pi(_mm_loadl_pi(tmp1, (__m64*)(dst.values+2)), (__m64*)(dst.values+6));
    row3 = _mm_loadh_pi(_mm_loadl_pi(row3, (__m64*)(dst.values+10)), (__m64*)(dst.values+14));
    row2 = _mm_shuffle_ps(tmp1, row3, 0x88);
    row3 = _mm_shuffle_ps(row3, tmp1, 0xDD);
    tmp1 = _mm_mul_ps(row2, row3);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
    minor0 = _mm_mul_ps(row1, tmp1);
    minor1 = _mm_mul_ps(row0, tmp1);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
    minor0 = _mm_sub_ps(_mm_mul_ps(row1, tmp1), minor0);
    minor1 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor1);
    minor1 = _mm_shuffle_ps(minor1, minor1, 0x4E);
    tmp1 = _mm_mul_ps(row1, row2);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
    minor0 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor0);
    minor3 = _mm_mul_ps(row0, tmp1);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
    minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row3, tmp1));
    minor3 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor3);
    minor3 = _mm_shuffle_ps(minor3, minor3, 0x4E);
    tmp1 = _mm_mul_ps(_mm_shuffle_ps(row1, row1, 0x4E), row3);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
    row2 = _mm_shuffle_ps(row2, row2, 0x4E);
    minor0 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor0);
    minor2 = _mm_mul_ps(row0, tmp1);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
    minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row2, tmp1));
    minor2 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor2);
    minor2 = _mm_shuffle_ps(minor2, minor2, 0x4E);
    tmp1 = _mm_mul_ps(row0, row1);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
    minor2 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor2);
    minor3 = _mm_sub_ps(_mm_mul_ps(row2, tmp1), minor3);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
    minor2 = _mm_sub_ps(_mm_mul_ps(row3, tmp1), minor2);
    minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row2, tmp1));
    tmp1 = _mm_mul_ps(row0, row3);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
    minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row2, tmp1));
    minor2 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor2);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
    minor1 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor1);
    minor2 = _mm_sub_ps(minor2, _mm_mul_ps(row1, tmp1));
    tmp1 = _mm_mul_ps(row0, row2);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
    minor1 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor1);
    minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row1, tmp1));
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
    minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row3, tmp1));
    minor3 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor3);
    det = _mm_mul_ps(row0, minor0);
    det = _mm_add_ps(_mm_shuffle_ps(det, det, 0x4E), det);
    det = _mm_add_ss(_mm_shuffle_ps(det, det, 0xB1), det);
    tmp1 = _mm_rcp_ss(det);
    det = _mm_sub_ss(_mm_add_ss(tmp1, tmp1), _mm_mul_ss(det, _mm_mul_ss(tmp1, tmp1)));
    det = _mm_shuffle_ps(det, det, 0x00);
    minor0 = _mm_mul_ps(det, minor0);
    _mm_storel_pi((__m64*)(dst.values), minor0);
    _mm_storeh_pi((__m64*)(dst.values+2), minor0);
    minor1 = _mm_mul_ps(det, minor1);
    _mm_storel_pi((__m64*)(dst.values+4), minor1);
    _mm_storeh_pi((__m64*)(dst.values+6), minor1);
    minor2 = _mm_mul_ps(det, minor2);
    _mm_storel_pi((__m64*)(dst.values+ 8), minor2);
    _mm_storeh_pi((__m64*)(dst.values+10), minor2);
    minor3 = _mm_mul_ps(det, minor3);
    _mm_storel_pi((__m64*)(dst.values+12), minor3);
    _mm_storeh_pi((__m64*)(dst.values+14), minor3);
#else
    Matrix4 dst, src = getTransposed();
    float tmp[12];
    /* calculate pairs for first 8 elements (cofactors) */
    tmp[0] = src.values[10] * src.values[15]; tmp[1] = src.values[11] * src.values[14];
    tmp[2] = src.values[9] * src.values[15]; tmp[3] = src.values[11] * src.values[13];
    tmp[4] = src.values[9] * src.values[14]; tmp[5] = src.values[10] * src.values[13];
    tmp[6] = src.values[8] * src.values[15]; tmp[7] = src.values[11] * src.values[12];
    tmp[8] = src.values[8] * src.values[14]; tmp[9] = src.values[10] * src.values[12];
    tmp[10] = src.values[8] * src.values[13]; tmp[11] = src.values[9] * src.values[12];
    /* calculate first 8 elements (cofactors) */
    dst.values[0] = tmp[0]*src.values[5] + tmp[3]*src.values[6] + tmp[4]*src.values[7];
    dst.values[0] -= tmp[1]*src.values[5] + tmp[2]*src.values[6] + tmp[5]*src.values[7];
    dst.values[1] = tmp[1]*src.values[4] + tmp[6]*src.values[6] + tmp[9]*src.values[7];
    dst.values[1] -= tmp[0]*src.values[4] + tmp[7]*src.values[6] + tmp[8]*src.values[7];
    dst.values[2] = tmp[2]*src.values[4] + tmp[7]*src.values[5] + tmp[10]*src.values[7];
    dst.values[2] -= tmp[3]*src.values[4] + tmp[6]*src.values[5] + tmp[11]*src.values[7];
    dst.values[3] = tmp[5]*src.values[4] + tmp[8]*src.values[5] + tmp[11]*src.values[6];
    dst.values[3] -= tmp[4]*src.values[4] + tmp[9]*src.values[5] + tmp[10]*src.values[6];
    dst.values[4] = tmp[1]*src.values[1] + tmp[2]*src.values[2] + tmp[5]*src.values[3];
    dst.values[4] -= tmp[0]*src.values[1] + tmp[3]*src.values[2] + tmp[4]*src.values[3];
    dst.values[5] = tmp[0]*src.values[0] + tmp[7]*src.values[2] + tmp[8]*src.values[3];
    dst.values[5] -= tmp[1]*src.values[0] + tmp[6]*src.values[2] + tmp[9]*src.values[3];
    dst.values[6] = tmp[3]*src.values[0] + tmp[6]*src.values[1] + tmp[11]*src.values[3];
    dst.values[6] -= tmp[2]*src.values[0] + tmp[7]*src.values[1] + tmp[10]*src.values[3];
    dst.values[7] = tmp[4]*src.values[0] + tmp[9]*src.values[1] + tmp[10]*src.values[2];
    dst.values[7] -= tmp[5]*src.values[0] + tmp[8]*src.values[1] + tmp[11]*src.values[2];
    /* calculate pairs for second 8 elements (cofactors) */
    tmp[0] = src.values[2]*src.values[7]; tmp[1] = src.values[3]*src.values[6];
    tmp[2] = src.values[1]*src.values[7]; tmp[3] = src.values[3]*src.values[5];
    tmp[4] = src.values[1]*src.values[6]; tmp[5] = src.values[2]*src.values[5];
    tmp[6] = src.values[0]*src.values[7]; tmp[7] = src.values[3]*src.values[4];
    tmp[8] = src.values[0]*src.values[6]; tmp[9] = src.values[2]*src.values[4];
    tmp[10] = src.values[0]*src.values[5]; tmp[11] = src.values[1]*src.values[4];
    /* calculate second 8 elements (cofactors) */
    dst.values[8] = tmp[0]*src.values[13] + tmp[3]*src.values[14] + tmp[4]*src.values[15];
    dst.values[8] -= tmp[1]*src.values[13] + tmp[2]*src.values[14] + tmp[5]*src.values[15];
    dst.values[9] = tmp[1]*src.values[12] + tmp[6]*src.values[14] + tmp[9]*src.values[15];
    dst.values[9] -= tmp[0]*src.values[12] + tmp[7]*src.values[14] + tmp[8]*src.values[15];
    dst.values[10] = tmp[2]*src.values[12] + tmp[7]*src.values[13] + tmp[10]*src.values[15];
    dst.values[10]-= tmp[3]*src.values[12] + tmp[6]*src.values[13] + tmp[11]*src.values[15];
    dst.values[11] = tmp[5]*src.values[12] + tmp[8]*src.values[13] + tmp[11]*src.values[14];
    dst.values[11]-= tmp[4]*src.values[12] + tmp[9]*src.values[13] + tmp[10]*src.values[14];
    dst.values[12] = tmp[2]*src.values[10] + tmp[5]*src.values[11] + tmp[1]*src.values[9];
    dst.values[12]-= tmp[4]*src.values[11] + tmp[0]*src.values[9] + tmp[3]*src.values[10];
    dst.values[13] = tmp[8]*src.values[11] + tmp[0]*src.values[8] + tmp[7]*src.values[10];
    dst.values[13]-= tmp[6]*src.values[10] + tmp[9]*src.values[11] + tmp[1]*src.values[8];
    dst.values[14] = tmp[6]*src.values[9] + tmp[11]*src.values[11] + tmp[3]*src.values[8];
    dst.values[14]-= tmp[10]*src.values[11] + tmp[2]*src.values[8] + tmp[7]*src.values[9];
    dst.values[15] = tmp[10]*src.values[10] + tmp[4]*src.values[8] + tmp[9]*src.values[9];
    dst.values[15]-= tmp[8]*src.values[9] + tmp[11]*src.values[10] + tmp[5]*src.values[8];
    /* calculate matrix inverse */
    float det = 1.0/(src.values[0]*dst.values[0]+src.values[1]*dst.values[1]+src.values[2]*dst.values[2]+src.values[3]*dst.values[3]);
    for(char i = 0; i < 16; i++)
        dst.values[i] *= det;
#endif
    return dst;
}

Matrix4 Matrix4::operator*(const Matrix4& b) {
    Matrix4 a;
#if defined (BT_USE_SSE)
    //http://fhtr.blogspot.de/2010/02/4x4-float-matrix-multiplication-using.html
    __m128 a_line, b_line, r_line;
    for(char i = 0; i < 16; i += 4) {
        a_line = _mm_set1_ps(values[i]);
        b_line = _mm_load_ps(b.values);
        r_line = _mm_mul_ps(b_line, a_line);
        for(char j = 1; j < 4; j ++) {
            a_line = _mm_set1_ps(values[i+j]);
            b_line = _mm_load_ps(&b.values[j*4]);
            r_line = _mm_add_ps(_mm_mul_ps(b_line, a_line), r_line);
        }
        _mm_store_ps(&a.values[i], r_line);
    }
#else
    a.values[0] = values[0]*b.values[0]+values[1]*b.values[4]+values[2]*b.values[8]+values[3]*b.values[12];
    a.values[1] = values[0]*b.values[1]+values[1]*b.values[5]+values[2]*b.values[9]+values[3]*b.values[13];
    a.values[2] = values[0]*b.values[2]+values[1]*b.values[6]+values[2]*b.values[10]+values[3]*b.values[14];
    a.values[3] = values[0]*b.values[3]+values[1]*b.values[7]+values[2]*b.values[11]+values[3]*b.values[15];
    a.values[4] = values[4]*b.values[0]+values[5]*b.values[4]+values[6]*b.values[8]+values[7]*b.values[12];
    a.values[5] = values[4]*b.values[1]+values[5]*b.values[5]+values[6]*b.values[9]+values[7]*b.values[13];
    a.values[6] = values[4]*b.values[2]+values[5]*b.values[6]+values[6]*b.values[10]+values[7]*b.values[14];
    a.values[7] = values[4]*b.values[3]+values[5]*b.values[7]+values[6]*b.values[11]+values[7]*b.values[15];
    a.values[8] = values[8]*b.values[0]+values[9]*b.values[4]+values[10]*b.values[8]+values[11]*b.values[12];
    a.values[9] = values[8]*b.values[1]+values[9]*b.values[5]+values[10]*b.values[9]+values[11]*b.values[13];
    a.values[10] = values[8]*b.values[2]+values[9]*b.values[6]+values[10]*b.values[10]+values[11]*b.values[14];
    a.values[11] = values[8]*b.values[3]+values[9]*b.values[7]+values[10]*b.values[11]+values[11]*b.values[15];
    a.values[12] = values[12]*b.values[0]+values[13]*b.values[4]+values[14]*b.values[8]+values[15]*b.values[12];
    a.values[13] = values[12]*b.values[1]+values[13]*b.values[5]+values[14]*b.values[9]+values[15]*b.values[13];
    a.values[14] = values[12]*b.values[2]+values[13]*b.values[6]+values[14]*b.values[10]+values[15]*b.values[14];
    a.values[15] = values[12]*b.values[3]+values[13]*b.values[7]+values[14]*b.values[11]+values[15]*b.values[15];
#endif
    return a;
}

Matrix4& Matrix4::operator*=(const Matrix4& b) {
    *this = *this * b;
    return *this;
}

Matrix4& Matrix4::operator=(const Matrix4& mat) {
    memcpy(values, mat.values, sizeof(values));
    return *this;
}

Matrix4& Matrix4::scale(btVector3 vec) {
    Matrix4 a;
    a.setIdentity();
    a.data.x.setX(vec.x());
    a.data.y.setY(vec.y());
    a.data.z.setZ(vec.z());
    *this = a;
    return *this;
}

Matrix4& Matrix4::makeTextureMat() {
    Matrix4 a;
    a.setIdentity();
    a.data.x.setX(0.5);
    a.data.y.setY(0.5);
    a.data.w = btVector3(0.5, 0.5, 0.0);
    *this = a;
    return *this;
}

Matrix4& Matrix4::perspective(float fovy, float aspect, float n, float f) {
    float a = 1.0/tan(fovy*0.5);
    Matrix4 b;
    b.setIdentity();
    b.data.x.setX(a/aspect);
    b.data.y.setY(a);
    b.data.z.setZ((n+f)/(n-f));
    b.data.z.setW(-1.0);
    b.data.w.setZ((2.0*n*f)/(n-f));
    b.data.w.setW(0.0);
    return (*this *= b);
}

Matrix4& Matrix4::frustum(float w, float h, float n, float f) {
    Matrix4 b;
    b.setIdentity();
    b.data.x.setX(n/w);
    b.data.y.setY(n/h);
    b.data.z.setZ((n+f)/(n-f));
    b.data.z.setW(-1.0);
    b.data.w.setZ((2.0*n*f)/(n-f));
    b.data.w.setW(0.0);
    return (*this *= b);
}

Matrix4& Matrix4::ortho(float w, float h, float n, float f) {
    Matrix4 b;
    b.setIdentity();
    b.data.x.setX(1.0/w);
    b.data.y.setY(1.0/h);
    b.data.z.setZ(2.0/(n-f));
    b.data.w.setZ((n+f)/(n-f));
    return (*this *= b);
}