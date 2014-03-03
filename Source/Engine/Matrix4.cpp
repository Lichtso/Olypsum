//
//  Matrix4.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 21.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "Matrix4.h"

Matrix4::Matrix4() {
    
}

Matrix4::Matrix4(Matrix4 const &mat) {
    x = mat.x;
    y = mat.y;
    z = mat.z;
    w = mat.w;
}

Matrix4::Matrix4(btMatrix3x3 const &basis)
    :x(basis.getColumn(0)), y(basis.getColumn(1)), z(basis.getColumn(2)), w(btVector3(0.0, 0.0, 0.0)) {
    w.setW(1.0);
}

Matrix4::Matrix4(btMatrix3x3 const &basis, btVector3 const &origin)
    :x(basis.getColumn(0)), y(basis.getColumn(1)), z(basis.getColumn(2)), w(origin) {
    w.setW(1.0);
}

Matrix4::Matrix4(btTransform const &mat) {
    btMatrix3x3 basis = mat.getBasis();
    x = basis.getColumn(0);
    y = basis.getColumn(1);
    z = basis.getColumn(2);
    w = mat.getOrigin();
    w.setW(1.0);
}

Matrix4::Matrix4(btScalar matData[16]) {
    memcpy(values, matData, sizeof(values));
}

btVector3 Matrix4::getColum(unsigned char index) {
    btVector3 vec;
    vec.setX(values[index  ]);
    vec.setY(values[index+4]);
    vec.setZ(values[index+8]);
    vec.setW(values[index+12]);
    return vec;
}

btMatrix3x3 Matrix4::getNormalMatrix() const {
    return btMatrix3x3(x.normalized(), y.normalized(), z.normalized()).transpose();
}

btMatrix3x3 Matrix4::getBTMatrix3x3() const {
    return btMatrix3x3(x, y, z).transpose();
}

btTransform Matrix4::getBTTransform() const {
    btTransform mat;
    mat.setBasis(getBTMatrix3x3());
    mat.setOrigin(w);
    return mat;
}

void Matrix4::getOpenGLMatrix(btScalar* matData) const {
    matData[0] = x.x();
    matData[1] = x.y();
    matData[2] = x.z();
    matData[3] = x.w();
    matData[4] = y.x();
    matData[5] = y.y();
    matData[6] = y.z();
    matData[7] = y.w();
    matData[8] = z.x();
    matData[9] = z.y();
    matData[10] = z.z();
    matData[11] = z.w();
    matData[12] = w.x();
    matData[13] = w.y();
    matData[14] = w.z();
    matData[15] = w.w();
}

bool Matrix4::isValid() {
    return isValidVector(x) && isValidVector(y) && isValidVector(z) && isValidVector(w);
}

Matrix4& Matrix4::setIdentity() {
    x = btVector3(1, 0, 0);
    y = btVector3(0, 1, 0);
    z = btVector3(0, 0, 1);
    w = btVector3(0, 0, 0);
    w.setW(1);
    return *this;
}

Matrix4 Matrix4::getTransposed() const {
    Matrix4 a;
    a.x.setX(x.x());
    a.x.setY(y.x());
    a.x.setZ(z.x());
    a.x.setW(w.x());
    a.y.setX(x.y());
    a.y.setY(y.y());
    a.y.setZ(z.y());
    a.y.setW(w.y());
    a.z.setX(x.z());
    a.z.setY(y.z());
    a.z.setZ(z.z());
    a.z.setW(w.z());
    a.w.setX(x.w());
    a.w.setY(y.w());
    a.w.setZ(z.w());
    a.w.setW(w.w());
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
        b_line = b.x.mVec128;
        r_line = _mm_mul_ps(b_line, a_line);
        for(char j = 1; j < 4; j ++) {
            a_line = _mm_set1_ps(values[i+j]);
            b_line = b.rows[j];
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

static inline btScalar dotVector4(const btVector3& a, const btVector3& b) {
    return a.x()*b.x() + a.y()*b.y() + a.z()*b.z() + a.w()*b.w();
}

btVector3 Matrix4::operator()(const btVector3& vec) {
    btVector3 resVec;
#if defined (BT_USE_SSE)
    //http://fastcpp.blogspot.de/2011/03/matrix-vector-multiplication-using-sse3.html
    __m128 x = _mm_loadu_ps((const float*)&vec.m_floats);
    //btVector3 colum = getColum(0);
    __m128 A0 = getColum(0).mVec128; // _mm_loadu_ps((const float*)&colum.m_floats);
    __m128 A1 = getColum(1).mVec128; // _mm_loadu_ps((const float*)&colum.m_floats);
    __m128 A2 = getColum(2).mVec128; // _mm_loadu_ps((const float*)&colum.m_floats);
    __m128 A3 = getColum(3).mVec128; // _mm_loadu_ps((const float*)&colum.m_floats);
    __m128 m0 = _mm_mul_ps(A0, x);
    __m128 m1 = _mm_mul_ps(A1, x);
    __m128 m2 = _mm_mul_ps(A2, x);
    __m128 m3 = _mm_mul_ps(A3, x);
    __m128 sum_01 = _mm_hadd_ps(m0, m1);
    __m128 sum_23 = _mm_hadd_ps(m2, m3);
    resVec.mVec128 = _mm_hadd_ps(sum_01, sum_23);
    //_mm_storeu_ps((float*)&resVec.m_floats, result);
#else
    resVec.setX(dotVector4(getColum(0), vec));
    resVec.setY(dotVector4(getColum(1), vec));
    resVec.setZ(dotVector4(getColum(2), vec));
    resVec.setW(dotVector4(getColum(3), vec));
#endif
    return resVec;
}

Matrix4& Matrix4::reflect(const btVector3& vec) {
    Matrix4 a;
    a.setIdentity();
    a.x.setX(1.0-2*vec.x()*vec.x());
    a.x.setY(-2.0*vec.x()*vec.y());
    a.x.setZ(-2.0*vec.x()*vec.z());
    a.y.setX(-2*vec.y()*vec.x());
    a.y.setY(1.0-2.0*vec.y()*vec.y());
    a.y.setZ(-2.0*vec.y()*vec.z());
    a.z.setX(-2*vec.z()*vec.x());
    a.z.setY(-2.0*vec.z()*vec.y());
    a.z.setZ(1.0-2.0*vec.z()*vec.z());
    return (*this *= a);
}

Matrix4& Matrix4::scale(const btVector3& vec) {
    Matrix4 a;
    a.setIdentity();
    a.x.setX(vec.x());
    a.y.setY(vec.y());
    a.z.setZ(vec.z());
    return (*this *= a);
}

Matrix4& Matrix4::rotate(const btQuaternion& quaternion) {
    btMatrix3x3 mat;
    mat.setRotation(quaternion);
    Matrix4 a(mat);
    return (*this *= a);
}

Matrix4& Matrix4::rotate(const btVector3& vec, btScalar angle) {
    btScalar s = sin(angle), c = cos(angle), d = 1.0-c;
    Matrix4 a;
    a.setIdentity();
    a.x.setX(vec.x()*vec.x()*d+c);
    a.x.setY(vec.x()*vec.y()*d-vec.z()*s);
    a.x.setZ(vec.x()*vec.z()*d+vec.y()*s);
    a.y.setX(vec.y()*vec.x()*d+vec.z()*s);
    a.y.setY(vec.y()*vec.y()*d+c);
    a.y.setZ(vec.y()*vec.z()*d-vec.x()*s);
    a.z.setX(vec.z()*vec.x()*d-vec.y()*s);
    a.z.setY(vec.z()*vec.y()*d+vec.x()*s);
    a.z.setZ(vec.z()*vec.z()*d+c);
    return (*this *= a);
}

Matrix4& Matrix4::translate(const btVector3& vec) {
    Matrix4 a;
    a.setIdentity();
    a.w.setX(vec.x());
    a.w.setY(vec.y());
    a.w.setZ(vec.z());
    return (*this *= a);
}

Matrix4& Matrix4::makeTextureMat() {
    Matrix4 a;
    a.setIdentity();
    a.x.setX(0.5);
    a.y.setY(0.5);
    a.w = btVector3(0.5, 0.5, 0.0);
    a.w.setW(1.0);
    return (*this *= a);
}

Matrix4& Matrix4::perspective(float fovy, float aspect, float n, float f) {
    float a = 1.0/tan(fovy*0.5);
    Matrix4 b;
    b.setIdentity();
    b.x.setX(a/aspect);
    b.y.setY(a);
    b.z.setZ((n+f)/(n-f));
    b.z.setW(-1.0);
    b.w.setZ((2.0*n*f)/(n-f));
    b.w.setW(0.0);
    return (*this *= b);
}

Matrix4& Matrix4::frustum(float w, float h, float n, float f) {
    Matrix4 b;
    b.setIdentity();
    b.x.setX(n/w);
    b.y.setY(n/h);
    b.z.setZ((n+f)/(n-f));
    b.z.setW(-1.0);
    b.w.setZ((2.0*n*f)/(n-f));
    b.w.setW(0.0);
    return (*this *= b);
}

Matrix4& Matrix4::ortho(float w, float h, float n, float f) {
    Matrix4 b;
    b.setIdentity();
    b.x.setX(1.0/w);
    b.y.setY(1.0/h);
    b.z.setZ(2.0/(n-f));
    b.w.setZ((n+f)/(n-f));
    return (*this *= b);
}

Matrix4 Matrix4::getInterpolation(const Matrix4& _b, float t) {
    btTransform a = getBTTransform(), b = _b.getBTTransform();
    btQuaternion rotA, rotB;
    a.getBasis().getRotation(rotA);
    b.getBasis().getRotation(rotB);
    Matrix4 result = btMatrix3x3(rotA.slerp(rotB, t));
    result.w = a.getOrigin().lerp(b.getOrigin(), t);
    return result;
}



std::string stringOf(Matrix4& mat) {
    std::ostringstream ss;
    ss << "(";
    for(unsigned char i = 0; i < 16; i ++) {
        if(i % 4 > 0) ss << " ";
        ss << mat.values[i];
        if(i % 4 == 3 && i < 15) ss << ",\n ";
    }
    ss << ")";
    return ss.str();
}

bool isValidVector(const btVector3& vec) {
    return isfinite(vec.x()) && isfinite(vec.y()) && isfinite(vec.z()) && isfinite(vec.w());
}