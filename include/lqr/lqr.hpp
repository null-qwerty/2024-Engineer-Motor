#pragma once

#include <eigen3/Eigen/Core>

class LQR{
public:
    LQR(Eigen::MatrixXf A, Eigen::MatrixXf B, Eigen::MatrixXf Q, Eigen::MatrixXf R);
    Eigen::MatrixXf getK();
    Eigen::MatrixXf getF();
    Eigen::MatrixXf getA();
    Eigen::MatrixXf getB();
    Eigen::MatrixXf getQ();
    Eigen::MatrixXf getR();
    void setA(Eigen::MatrixXf A);
    void setB(Eigen::MatrixXf B);
    void setQ(Eigen::MatrixXf Q);
    void setR(Eigen::MatrixXf R);
    void setK(Eigen::MatrixXf K);
    void setF(Eigen::MatrixXf F);
    Eigen::MatrixXf solve();
private:
    Eigen::MatrixXf A;
    Eigen::MatrixXf B;
    Eigen::MatrixXf Q;
    Eigen::MatrixXf R;
    Eigen::MatrixXf K;
    Eigen::MatrixXf F;
};