#pragma once

struct Point {
  float data[3];

  float& operator[](int i) { return data[i]; }
  const float& operator[](int i) const { return data[i]; }
  Point operator-() const { return {-data[0], -data[1], -data[2]}; }
};

constexpr Point operator+(const Point& a, const Point& b) {
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
