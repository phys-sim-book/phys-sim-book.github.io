#pragma once

#include <memory>
#include <Eigen/Dense>
#include "uti.h"

template <typename T, int dim>
class BarrierEnergy
{
public:
    BarrierEnergy(const std::vector<T> &x, const std::vector<T> &contact_area, T y_ground);
    BarrierEnergy();
    ~BarrierEnergy();
    BarrierEnergy(BarrierEnergy &&rhs);
    BarrierEnergy(const BarrierEnergy &rhs);
    BarrierEnergy &operator=(BarrierEnergy &&rhs);

    void update_x(const DeviceBuffer<T> &x);
    T val();                                    // Calculate the value of the energy
    const DeviceBuffer<T> &grad();              // Calculate the gradient of the energy
    const DeviceTripletMatrix<T, 1> &hess();    // Calculate the Hessian matrix of the energy
    T init_step_size(const DeviceBuffer<T> &p); // Calculate the initial step size for the line search

private:
    // The implementation details of the VecAdder class are placed in the implementation class declared here.
    struct Impl;
    // The private pointer to the implementation class Impl
    std::unique_ptr<Impl> pimpl_;
};