#include <cmath>
#include <iostream>
#include "environment.h"
#include "config.h"

Environment::Environment(const int n, const int lmd, const int l, const float alpha, const float beta)
{
    this->n = n;
    this->l = l;
    this->alpha = alpha;
    this->beta = beta;

    this->hub_links.resize(n, false);
    if (lmd != 0) {
        int quotient = n / lmd;
        int remainder = n % lmd;
        int node_n = 0;
        for (unsigned i = 0; i < lmd; ++i) {
            this->hub_links.at(node_n) = true;
            if (remainder > 0) {
                node_n += quotient + 1;
                --remainder;
            } else {
                node_n += quotient;
            }
        }
    }
}

float Environment::compute_cost_out(const int origin, const int destination)
{
    return std::min(std::abs(origin - destination), std::min(origin, this->n - origin) + std::min(destination, this->n - destination));
}

int Environment::determine_new_state(const int attendance)
{
    int new_state;
    if (attendance > this->l) {
        new_state = CONGESTED;
    } else { // attendance <= this->l
        new_state = NON_CONGESTED;
    }
    this->congestion_history.emplace_back(new_state);
    this->attendance_history.emplace_back(attendance);
    return new_state;
}

std::vector<float> Environment::compute_costs_in(const int origin, const int destination)
{
    /* Find the entrances to the hub
        o_l:
        o_r:
        d_l:
        d_r:
    */
    int i, o_l, o_r, d_l, d_r, dis_o_l, dis_o_r, dis_d_l, dis_d_r;
    if (origin < destination) {
        // Find o_l and dis_o_l
        for (i = origin; i < destination; ++i) {
            if (this->hub_links.at(i)) {
                o_l = i;
                dis_o_l = i - origin;
                break;
            }
        }
        if (i == destination) {
            o_l = -1;
            dis_o_l = -1;
        }
        // Find o_r and dis_o_r
        for (i = origin; i >= 0; --i) {
            if (this->hub_links.at(i)) {
                o_r = i;
                dis_o_r = origin - i;
                break;
            }
        }
        if (i == -1) {
            for (i = this->n - 1; i > destination; --i) {
                if (this->hub_links.at(i)) {
                    o_r = i;
                    dis_o_r = origin - (i - this->n);
                    break;
                }
            }
            if (i == destination) {
                o_r = -1;
                dis_o_r = -1;
            }
        }
        // Find d_l and dis_d_l
        for (i = destination; i > origin; --i) {
            if (this->hub_links.at(i)) {
                d_l = i;
                dis_d_l = destination - i;
                break;
            }
        }
        if (i == origin) {
            d_l = -1;
            dis_d_l = -1;
        }
        // Find d_r and dis_d_r
        for (i = destination; i < this->n; ++i) {
            if (this->hub_links.at(i)) {
                d_r = i;
                dis_d_r = i - destination;
                break;
            }
        }
        if (i == this->n) {
            for (i = 0; i < origin; ++i) {
                if (this->hub_links.at(i)) {
                    d_r = i;
                    dis_d_r = (this->n + i) - destination;
                    break;
                }
            }
            if (i == origin) {
                d_r = -1;
                dis_d_r = -1;
            }
        }
    } else {
        // Find o_l and dis_o_l
        for (i = origin; i < this->n; ++i) {
            if (this->hub_links.at(i)) {
                o_l = i;
                dis_o_l = i - origin;
                break;
            }
        }
        if (i == this->n) {
            for (i = 0; i < destination; ++i) {
                if (this->hub_links.at(i)) {
                    o_l = i;
                    dis_o_l = (this->n + i) - origin;
                    break;
                }
            }
            if (i == destination) {
                o_l = -1;
                dis_o_l = -1;
            }
        }
        // Find o_r and dis_o_r
        for (i = origin; i > destination; --i) {
            if (this->hub_links.at(i)) {
                o_r = i;
                dis_o_r = origin - i;
                break;
            }
        }
        if (i == destination) {
            o_r = -1;
            dis_o_r = -1;
        }
        // Find d_l and dis_d_l
        for (i = destination; i >= 0; --i) {
            if (this->hub_links.at(i)) {
                d_l = i;
                dis_d_l = destination - i;
                break;
            }
        }
        if (i == -1) {
            for (i = this->n - 1; i > origin; --i) {
                if (this->hub_links.at(i)) {
                    d_l = i;
                    dis_d_l = destination - (i - this->n);
                    break;
                }
            }
            if (i == origin) {
                d_l = -1;
                dis_d_l = -1;
            }
        }
        // Find d_r and dis_d_r
        for (i = destination; i < origin; ++i) {
            if (this->hub_links.at(i)) {
                d_r = i;
                dis_d_r = i - destination;
                break;
            }
        }
        if (i == origin) {
            d_r = -1;
            dis_d_r = -1;
        }
    }
    /* Compute cost
        cost_in_con_ll:
    */
    float cost_in_con_ll, cost_in_con_lr, cost_in_con_rl, cost_in_con_rr;
    float cost_in_ncon_ll, cost_in_ncon_lr, cost_in_ncon_rl, cost_in_ncon_rr;
    float cost_in_con = this->n * 10;
    float cost_in_ncon = this->n * 10;
    int shortcut;

    // Compute cost_in_ll
    if (o_l != -1 && d_l != -1 && o_l != d_l) {
        cost_in_con_ll = dis_o_l + this->beta * std::min(std::abs(d_l - o_l), (this->n - std::abs(d_l - o_l))) + dis_d_l;
        cost_in_ncon_ll = dis_o_l + this->alpha * std::min(std::abs(d_l - o_l), (this->n - std::abs(d_l - o_l))) + dis_d_l;
        if (cost_in_ncon_ll < cost_in_ncon) {
            shortcut = std::min(std::abs(d_l - o_l), (this->n - std::abs(d_l - o_l))); // for nash equi.
            cost_in_con = cost_in_con_ll;
            cost_in_ncon = cost_in_ncon_ll;
        }
    }
    // Compute cost_in_lr
    if (o_l != -1 && d_r != -1) {
        cost_in_con_lr = dis_o_l + this->beta * std::min(std::abs(d_r - o_l), (this->n - std::abs(d_r - o_l))) + dis_d_r;
        cost_in_ncon_lr = dis_o_l + this->alpha * std::min(std::abs(d_r - o_l), (this->n - std::abs(d_r - o_l))) + dis_d_r;
        if (cost_in_ncon_lr < cost_in_ncon) {
            shortcut = std::min(std::abs(d_r - o_l), (this->n - std::abs(d_r - o_l)));
            cost_in_con = cost_in_con_lr;
            cost_in_ncon = cost_in_ncon_lr;
        }
    }
    // Compute cost_rl
    if (o_r != -1 && d_l != -1) {
        cost_in_con_rl = dis_o_r + this->beta * std::min(std::abs(d_l - o_r), (this->n - std::abs(d_l - o_r))) + dis_d_l;
        cost_in_ncon_rl = dis_o_r + this->alpha * std::min(std::abs(d_l - o_r), (this->n - std::abs(d_l - o_r))) + dis_d_l;
        if (cost_in_ncon_rl < cost_in_ncon) {
            shortcut = std::min(std::abs(d_l - o_r), (this->n - std::abs(d_l - o_r)));
            cost_in_con = cost_in_con_rl;
            cost_in_ncon = cost_in_ncon_rl;
        }
    }
    // Compute cost_rr
    if (o_r != -1 && d_r != -1 && o_r != d_r) {
        cost_in_con_rr = dis_o_r + this->beta * std::min(std::abs(d_r - o_r), (this->n - std::abs(d_r - o_r))) + dis_d_r;
        cost_in_ncon_rr = dis_o_r + this->alpha * std::min(std::abs(d_r - o_r), (this->n - std::abs(d_r - o_r))) + dis_d_r;
        if (cost_in_ncon_rr < cost_in_ncon) {
            shortcut = std::min(std::abs(d_r - o_r), (this->n - std::abs(d_r - o_r)));
            cost_in_con = cost_in_con_rr;
            cost_in_ncon = cost_in_ncon_rr;
        }
    }
    std::vector<float> costs_in = {cost_in_con, cost_in_ncon};
    return costs_in;
}

std::vector<int>& Environment::get_congestion_history()
{
    return this->congestion_history;
}

void Environment::update_congestion_history(const int new_state)
{
    this->congestion_history.emplace_back(new_state);
}

void Environment::update_attendance_history(const int attendance)
{
    this->attendance_history.emplace_back(attendance);
}

std::vector<int> Environment::get_recent_congestion_history(const int m)
{
    std::vector<int> recent_congestion_history;

    if (m > this->congestion_history.size()) {
        for (unsigned i = 0; i < m; ++i) {
            recent_congestion_history.emplace_back(rng() % 2);
        }
    } else {
        for (auto iter = this->congestion_history.end() - m; iter != this->congestion_history.end(); ++iter) {
            recent_congestion_history.emplace_back(*iter);
        }
    }

    return recent_congestion_history;
}

std::vector<int>& Environment::get_attendance_history()
{
    return this->attendance_history;
}

std::ostream& operator<<(std::ostream& os, const Environment& environment)
{
    // hub capacity
    os << "hub capacity (l):" << std::endl;
    os << "\t" << environment.l << std::endl;
    // hub links
    os << "hub links:" << std::endl;
    os << "\t";
    for (auto hub_link : environment.hub_links) {
        os << hub_link << " ";
    }
    os << std::endl;
    os << "congestion history:" << std::endl;
    os << "\t";
    for (auto state : environment.congestion_history) {
        os << state << " ";
    }
    os << std::endl;
    os << "attendance history:" << std::endl;
    os << "\t";
    for (auto attendance : environment.attendance_history) {
        os << attendance << " ";
    }
    return os;
}
