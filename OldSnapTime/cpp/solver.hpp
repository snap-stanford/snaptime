#ifndef SOLVER_H_
#define SOLVER_H_

#include <vector>
#include <map>
#include <set>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include "Eigen/Core"
#include "Eigen/Eigen"
#define PI 3.14159265358979323846

class Solver
{
    protected:
        std::vector<Eigen::MatrixXd> Theta;
        std::vector<Eigen::MatrixXd> mu;
        std::vector<Eigen::MatrixXd> Z;
        std::vector<Eigen::MatrixXd> U;
        std::vector<Eigen::MatrixXd> S;
        std::vector<std::vector<Eigen::MatrixXd> > assignments;
        std::vector<int> current_optimal;
        double p;
        Eigen::MatrixXd LLE;

        int sweep_length; //contiguous points assigned to the empty cluster
        int K; //number of clusters
        double beta; //switching penalty
        double rho; //regularisation constant
        Eigen::MatrixXd data; //data points in nw x nw format
        Eigen::MatrixXd lambda; //sparseness matrix
        int n; //number of sensors
        int w; //window size
        double e_abs; //convergence param
        double e_rel; //convergence param
        int ADMM_iter;
        void computeLLE();
        void Estep();
        void Mstep();

    public:
        Solver(int sweep_length, int K, double beta, double rho, Eigen::MatrixXd data, Eigen::MatrixXd lambda, int n, int w,std::vector<Eigen::MatrixXd> init_mu,std::vector<Eigen::MatrixXd> init_theta, double e_abs, double e_rel, int ADMM_iter) {
            this->sweep_length = sweep_length;
            this->K = K;
            this->beta = beta;
            this->rho = rho;
            this->data = data;
            this->lambda = lambda;
            this->n = n;
            this->w = w;
            this->p = sqrt(n*w*1.0*(n*w+1)/2);
            for(int i = 0 ; i < K ;++i){
                Theta.push_back(init_theta[i]);
                Eigen::MatrixXd mat(n*w,n*w);
                mat.setZero(n*w,n*w);
                Z.push_back(mat);
                U.push_back(mat);
                S.push_back(mat);
                mu.push_back(init_mu[i]);
                std::vector<Eigen::MatrixXd> vec;
                vec.clear();
                assignments.push_back(vec);
            }
            this->e_abs = e_abs;
            this->e_rel = e_rel;
            this->ADMM_iter = ADMM_iter;
            current_optimal.clear();
            LLE = Eigen::MatrixXd(data.rows(),K);
            LLE.setZero(data.rows(),K);
        }

        void Solve(int steps) {
            bool converged = false;
            for(int i = 0 ; i < steps && !converged; ++i){
                auto old_optimal = current_optimal;
                Estep();
                if(i != 0){
                    converged = true;
                    for(int j = 0; j < old_optimal.size(); ++j){
                        if(old_optimal[j] != current_optimal[j]) {
                            converged = false;
                            break;
                        }
                    }
                }
                Mstep();
            }
        }

        Eigen::MatrixXd obtainTheta(int idx){
            return Theta[idx];
        }
        
        std::vector<int> obtainAssignment(){
            return current_optimal;
        }
};
            


#endif
