#include "uti.h"

using namespace muda;

// ANCHOR: add_vector
template <typename T>
DeviceBuffer<T> add_vector(const DeviceBuffer<T> &a, const DeviceBuffer<T> &b, const T &factor1, const T &factor2)
{

    int N = a.size();
    DeviceBuffer<T> c_device(N);
    ParallelFor(256)
        .apply(N,
               [c_device = c_device.viewer(), a_device = a.cviewer(), b_device = b.cviewer(), factor1, factor2] __device__(int i) mutable
               {
                   c_device(i) = a_device(i) * factor1 + b_device(i) * factor2;
               })
        .wait();
    return c_device;
}
// ANCHOR_END: add_vector

template DeviceBuffer<float> add_vector<float>(const DeviceBuffer<float> &a, const DeviceBuffer<float> &b, const float &factor1, const float &factor2);
template DeviceBuffer<double> add_vector<double>(const DeviceBuffer<double> &a, const DeviceBuffer<double> &b, const double &factor1, const double &factor2);

template <typename T>
DeviceBuffer<T> mult_vector(const DeviceBuffer<T> &a, const T &b)
{
    int N = a.size();
    DeviceBuffer<T> c_device(N);
    ParallelFor(256)
        .apply(N,
               [c_device = c_device.viewer(), a_device = a.cviewer(), b] __device__(int i) mutable
               {
                   c_device(i) = a_device(i) * b;
               })
        .wait();
    return c_device;
}
template DeviceBuffer<float> mult_vector<float>(const DeviceBuffer<float> &a, const float &b);
template DeviceBuffer<double> mult_vector<double>(const DeviceBuffer<double> &a, const double &b);

template <typename T>
DeviceTripletMatrix<T, 1> add_triplet(const DeviceTripletMatrix<T, 1> &a, const DeviceTripletMatrix<T, 1> &b, const T &factor1, const T &factor2)
{
    int Na = a.triplet_count();
    int Nb = b.triplet_count();
    DeviceTripletMatrix<T, 1> c;
    c.resize_triplets(Na + Nb);
    c.reshape(a.rows(), a.cols());
    ParallelFor(256)
        .apply(Na,
               [c_device_values = c.values().viewer(), c_device_row_indices = c.row_indices().viewer(), c_device_col_indices = c.col_indices().viewer(),
                a_device_values = a.values().cviewer(), a_device_row_indices = a.row_indices().cviewer(), a_device_col_indices = a.col_indices().cviewer(), factor1] __device__(int i) mutable
               {
                   c_device_row_indices(i) = a_device_row_indices(i);
                   c_device_col_indices(i) = a_device_col_indices(i);
                   c_device_values(i) = a_device_values(i) * factor1;
               })
        .wait();
    ParallelFor(256)
        .apply(Nb,
               [c_device_values = c.values().viewer(), c_device_row_indices = c.row_indices().viewer(), c_device_col_indices = c.col_indices().viewer(),
                b_device_values = b.values().cviewer(), b_device_row_indices = b.row_indices().cviewer(), b_device_col_indices = b.col_indices().cviewer(), factor2, Na] __device__(int i) mutable
               {
                   c_device_row_indices(i + Na) = b_device_row_indices(i);
                   c_device_col_indices(i + Na) = b_device_col_indices(i);
                   c_device_values(i + Na) = b_device_values(i) * factor2;
               })
        .wait();

    return c;
}
template DeviceTripletMatrix<float, 1> add_triplet<float>(const DeviceTripletMatrix<float, 1> &a, const DeviceTripletMatrix<float, 1> &b, const float &factor1, const float &factor2);
template DeviceTripletMatrix<double, 1> add_triplet<double>(const DeviceTripletMatrix<double, 1> &a, const DeviceTripletMatrix<double, 1> &b, const double &factor1, const double &factor2);
template <typename T>
T max_vector(const DeviceBuffer<T> &a)
{
    DeviceBuffer<T> buffer(a);
    T vec_max = 0.0f;              // Result of the reduction
    T *d_out;                      // Device memory to store the result of the reduction
    cudaMalloc(&d_out, sizeof(T)); // Allocate memory for the result
    int N = buffer.size();
    ParallelFor(256)
        .apply(N,
               [buffer = buffer.viewer()] __device__(int i) mutable
               {
                   buffer(i) = fabs(buffer(i));
               })
        .wait();
    DeviceReduce().Max(buffer.data(), d_out, buffer.size());

    // Copy the result back to the host
    cudaMemcpy(&vec_max, d_out, sizeof(T), cudaMemcpyDeviceToHost);

    // Clean up
    cudaFree(d_out);
    return vec_max;
}
template float max_vector<float>(const DeviceBuffer<float> &a);
template double max_vector<double>(const DeviceBuffer<double> &a);

// ANCHOR: search_dir
template <typename T>
void search_dir(const DeviceBuffer<T> &grad, const DeviceTripletMatrix<T, 1> &hess, DeviceBuffer<T> &dir)
{
    static LinearSystemContext ctx;
    auto neg_grad = mult_vector<T>(grad, -1);
    int N = grad.size();
    DeviceDenseVector<T> grad_device;
    grad_device.resize(N);
    grad_device.buffer_view().copy_from(neg_grad);
    DeviceCOOMatrix<T> A_coo;
    ctx.convert(hess, A_coo);
    DeviceCSRMatrix<T> A_csr;
    ctx.convert(A_coo, A_csr);
    DeviceDenseVector<T> dir_device;
    dir_device.resize(N);
    ctx.solve(dir_device.view(), A_csr.cview(), grad_device.cview());
    ctx.sync();
    dir.view().copy_from(dir_device.buffer_view());
}
// ANCHOR_END: search_dir
template void search_dir<float>(const DeviceBuffer<float> &grad, const DeviceTripletMatrix<float, 1> &hess, DeviceBuffer<float> &dir);
template void search_dir<double>(const DeviceBuffer<double> &grad, const DeviceTripletMatrix<double, 1> &hess, DeviceBuffer<double> &dir);

template <typename T>
void display_vec(const DeviceBuffer<T> &vec)
{
    int N = vec.size();
    ParallelFor(256)
        .apply(N,
               [vec = vec.cviewer()] __device__(int i) mutable
               {
                   printf("%d %f\n", i, vec(i));
               })
        .wait();
}
template void display_vec<float>(const DeviceBuffer<float> &vec);
template void display_vec<double>(const DeviceBuffer<double> &vec);