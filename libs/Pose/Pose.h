#pragma once

#include <Arduino.h>

struct Point2 {
  float data[2];

  float& operator[](int i) { return data[i]; }
  const float& operator[](int i) const { return data[i]; }
  Point2 operator-() const { return {-data[0], -data[1]}; }

  Point2 operator/(float x) const { return Point2{data[0] / x, data[1] / x}; }
  float dot(const Point2& other) const { return data[0] * other[0] + data[1] * other[1]; }
  float length() const { return sqrt(dot(*this)); }
  Point2 normalized() const { return *this / length(); }
};

struct Point {
  float data[3];

  float& operator[](int i) { return data[i]; }
  const float& operator[](int i) const { return data[i]; }
  Point operator-() const { return {-data[0], -data[1], -data[2]}; }

  float dot(const Point& other) const {
    return data[0] * other[0] + data[1] * other[1] + data[2] * other[2];
  }
  float length() const { return sqrt(dot(*this)); }
};

inline Point operator+(const Point& a, const Point& b) {
  return {a[0] + b[0], a[1] + b[1], a[2] + b[2]};
}

struct Matrix {
  float data[3][3];

  static Matrix identity() {
    Matrix matrix;
    for (int i = 0; i < 3; ++i)
      matrix.data[i][i] = 1.0;
    return matrix;
  }

  float determinant() const {
    return data[0][0] * (data[1][1] * data[2][2] - data[2][1] * data[1][2]) -
           data[0][1] * (data[1][0] * data[2][2] - data[1][2] * data[2][0]) +
           data[0][2] * (data[1][0] * data[2][1] - data[1][1] * data[2][0]);
  }

  Matrix inverse() const {
    float invdet = 1 / determinant();
    Matrix inv;
    inv.data[0][0] = (data[1][1] * data[2][2] - data[2][1] * data[1][2]) * invdet;
    inv.data[0][1] = (data[0][2] * data[2][1] - data[0][1] * data[2][2]) * invdet;
    inv.data[0][2] = (data[0][1] * data[1][2] - data[0][2] * data[1][1]) * invdet;
    inv.data[1][0] = (data[1][2] * data[2][0] - data[1][0] * data[2][2]) * invdet;
    inv.data[1][1] = (data[0][0] * data[2][2] - data[0][2] * data[2][0]) * invdet;
    inv.data[1][2] = (data[1][0] * data[0][2] - data[0][0] * data[1][2]) * invdet;
    inv.data[2][0] = (data[1][0] * data[2][1] - data[2][0] * data[1][1]) * invdet;
    inv.data[2][1] = (data[2][0] * data[0][1] - data[0][0] * data[2][1]) * invdet;
    inv.data[2][2] = (data[0][0] * data[1][1] - data[1][0] * data[0][1]) * invdet;
    return inv;
  }

  static Matrix fromEulerAngles(float xAngle, float yAngle, float zAngle) {
    Matrix matrix;

    float cx = cos(xAngle);
    float sx = sin(xAngle);
    float cy = cos(yAngle);
    float sy = sin(yAngle);
    float cz = cos(zAngle);
    float sz = sin(zAngle);

    matrix.data[0][0] = cy * cz;
    matrix.data[0][1] = -cy * sz;
    matrix.data[0][2] = sy;
    matrix.data[1][0] = cz * sx * sy + cx * sz;
    matrix.data[1][1] = cx * cz - sx * sy * sz;
    matrix.data[1][2] = -cy * sx;
    matrix.data[2][0] = -cx * cz * sy + sx * sz;
    matrix.data[2][1] = cz * sx + cx * sy * sz;
    matrix.data[2][2] = cx * cy;

    return matrix;
  }
};

inline Matrix operator*(const Matrix& a, const Matrix& b) {
  Matrix out{};
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      for (int k = 0; k < 3; ++k)
        out.data[i][k] = a.data[i][j] * b.data[j][k];
  return out;
}

inline Point operator*(const Matrix& a, const Point& p) {
  Point out{};
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      out[i] += a.data[i][j] * p[j];
  return out;
}

struct Pose {
  Matrix rotation;
  Point translation;

  static Pose identity() { return {Matrix::identity(), {}}; }

  Pose inverse() const {
    auto inverse_rotation = rotation.inverse();
    return {inverse_rotation, -(inverse_rotation * translation)};
  }
};

inline Point operator*(const Pose& a, const Point& p) { return a.rotation * p + a.translation; }

inline Pose operator*(const Pose& a, const Pose& b) {
  return {a.rotation * b.rotation, a.rotation * b.translation + a.translation};
}
