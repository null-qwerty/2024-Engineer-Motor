#pragma once
#include <algorithm>
#include <cmath>

class PID {
public:
    /**
     * @brief PID 控制器构造函数
     * 
     * @param Kp 比例增益
     * @param Ki 积分增益
     * @param Kd 微分增益
     * @param Umax 输出上限
     * @param Umin 输出下限
     * @param Kiae IAE 系数
     * @param Kr 参考值权重
     * @param Km 微分权重
     * 
     * * @note 设置滤波器系数需要调用 setFilter 函数，默认 c1 = 1.0, c2 = 0.0
     */
    PID(double Kp, double Ki, double Kd, double Umax, double Umin,
        double Kr = 1.0, double Km = 1.0, double Kiae = 0.0);
    /**
     * @brief PID 控制器析构函数
     */
    ~PID() = default;
    /**
     * @brief PID 控制器更新
     * 
     * @param Ref 目标值
     * @param Fbk 当前值
     * @return double 输出值
     */
    double Update(double Ref, double Fbk);
    /**
     * @brief 设置滤波器系数
     * 
     * @param a 截止频率 (Hz)
     * @param T 采样周期 (s)
     */
    void setFilter(double a, double T);

    double getKp() { return Kp; }
    double getKi() { return Ki; }
    double getKd() { return Kd; }

private:
    // PID param
    double Kr = 1.;
    double Kp;
    double Ki;
    double Kd;
    double Km = 1.;
    double Umax;
    double Umin;
    double Kiae;
    double c1 = 1., c2 = 0.; // 滤波器系数
    // PID data
    double pOut; // 比例输出
    double iOut; // 积分输出
    double dOut; // 微分输出
    double preSatOut; // 限幅前输出
    double last_iOut; // 上一次积分输出
    double last_dOut; // 上一次微分输出
    double dErr; // 微分误差
    double last_dErr; // 上一次微分误差
    double sat_record; // 限幅前后差值
    double last_Iae;
    int w1 = 1;

    double Err;
    double last_Err;
    double Out;
    double Iae;

    double x[3];
    double w[3];
    double w_sum;
    double u_;
    double last_u_;
    double pre_u_;
};

struct pidController : public PID {
    float target = 0;
    float observed = 0;
    float error = 0;
    float init_pos = 0;
    float max_limit = M_PI;
    float min_limit = 0.0;

    pidController(const PID &pid = PID(0, 0, 0, 0, 0))
        : PID(pid)
    {
    }
    float target_limit(float target)
    {
        if (target > max_limit)
            return max_limit;
        else if (target < min_limit)
            return min_limit;
        else
            return target;
    }
    void setLimit(float max, float min)
    {
        max_limit = max;
        min_limit = min;
    }
};