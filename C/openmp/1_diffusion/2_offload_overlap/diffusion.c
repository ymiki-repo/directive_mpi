

#include <stdio.h>
#include <math.h>


double diffusion3d(int nprocs, int rank, int nx, int ny, int nz, int mgn, float dx, float dy, float dz, float dt, float kappa,
                   const float *f, float *fn)
{
    const float ce = kappa*dt/(dx*dx);
    const float cw = ce;
    const float cn = kappa*dt/(dy*dy);
    const float cs = cn;
    const float ct = kappa*dt/(dz*dz);
    const float cb = ct;

    const float cc = 1.0F - (ce + cw + cn + cs + ct + cb);

    int k = 0;
#pragma omp target teams loop collapse(2)
    for (int j = 0; j < ny; j++) {
	for (int i = 0; i < nx; i++) {
	    const int ix = nx*ny*(k+mgn) + nx*j + i;
	    const int ip = i == nx - 1 ? ix : ix + 1;
	    const int im = i == 0      ? ix : ix - 1;
	    const int jp = j == ny - 1 ? ix : ix + nx;
	    const int jm = j == 0      ? ix : ix - nx;
	    const int kp = ix + nx*ny;
	    const int km = (rank == 0          && k == 0     ) ? ix : ix - nx*ny;

	    fn[ix] = cc*f[ix] + ce*f[ip] + cw*f[im] + cn*f[jp] + cs*f[jm] + ct*f[kp] + cb*f[km];
	}
    }
    k = nz-1;
#pragma omp target teams loop collapse(2)
    for (int j = 0; j < ny; j++) {
	for (int i = 0; i < nx; i++) {
	    const int ix = nx*ny*(k+mgn) + nx*j + i;
	    const int ip = i == nx - 1 ? ix : ix + 1;
	    const int im = i == 0      ? ix : ix - 1;
	    const int jp = j == ny - 1 ? ix : ix + nx;
	    const int jm = j == 0      ? ix : ix - nx;
	    const int kp = (rank == nprocs - 1 && k == nz - 1) ? ix : ix + nx*ny;
	    const int km = ix - nx*ny;

	    fn[ix] = cc*f[ix] + ce*f[ip] + cw*f[im] + cn*f[jp] + cs*f[jm] + ct*f[kp] + cb*f[km];
	}
    }

#pragma omp target teams loop collapse(3) nowait
    for(int k = 1; k < nz-1; k++) {
        for (int j = 0; j < ny; j++) {
            for (int i = 0; i < nx; i++) {
                const int ix = nx*ny*(k+mgn) + nx*j + i;
                const int ip = i == nx - 1 ? ix : ix + 1;
                const int im = i == 0      ? ix : ix - 1;
                const int jp = j == ny - 1 ? ix : ix + nx;
                const int jm = j == 0      ? ix : ix - nx;
                const int kp = ix + nx*ny;
                const int km = ix - nx*ny;

                fn[ix] = cc*f[ix] + ce*f[ip] + cw*f[im] + cn*f[jp] + cs*f[jm] + ct*f[kp] + cb*f[km];
            }
        }
    }

    return (double)(nx*ny*nz)*13.0;
}


void init(int nprocs, int rank, int nx, int ny, int nz, int mgn, float dx, float dy, float dz, float *f)
{
    const float kx = 2.0F*(float)M_PI;
    const float ky = kx;
    const float kz = kx;

    for(int k=-mgn; k < nz+mgn; k++) {
        for(int j=0; j < ny; j++) {
            for(int i=0; i < nx; i++) {
                const int ix = nx*ny*(k+mgn) + nx*j + i;
                const float x = dx*((float)i + 0.5F);
                const float y = dy*((float)j + 0.5F);
                const float z = dz*((float)(k + nz*rank) + 0.5F);

                f[ix] = 0.125F*(1.0F - cosf(kx*x))*(1.0F - cosf(ky*y))*(1.0F - cosf(kz*z));

            }
        }
    }
}

double err(int nprocs, int rank, double time, int nx, int ny, int nz, int mgn, float dx, float dy, float dz, float kappa, const float *f)
{
    const float kx = 2.0F*(float)M_PI;
    const float ky = kx;
    const float kz = kx;

    const float ax = expf(-kappa*(float)time*(kx*kx));
    const float ay = expf(-kappa*(float)time*(ky*ky));
    const float az = expf(-kappa*(float)time*(kz*kz));

    double ferr = 0.0;

    for(int k=0; k < nz; k++) {
        for(int j=0; j < ny; j++) {
            for(int i=0; i < nx; i++) {
                const int ix = nx*ny*(k+mgn) + nx*j + i;
                const float x = dx*((float)i + 0.5F);
                const float y = dy*((float)j + 0.5F);
                const float z = dz*((float)(k + nz*rank) + 0.5F);

                const float f0 = 0.125F*(1.0F - ax*cosf(kx*x)) * (1.0F - ay*cosf(ky*y)) * (1.0F - az*cosf(kz*z));

                const double diff = (double)f[ix] - (double)f0;
                ferr += diff * diff;
            }
        }
    }

    return ferr;
}

