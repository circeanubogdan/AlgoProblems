#include <fstream>
#include <iostream>
#include <cstddef>
#include <vector>
#include <cassert>

std::ifstream in("birocratie.in");
std::ofstream out("birocratie.out");

static constexpr int64_t MAX_N = 1010;
static constexpr int64_t MIN_VAL = (int64_t)-5000000;
static constexpr int32_t DIAG_UP = 0;
static constexpr int32_t DIAG_DOWN = 1;
int64_t dp[MAX_N][MAX_N][2] = {};
int64_t cost[MAX_N][MAX_N] = {};

enum Dir : int32_t {
    DOWN_LEFT,
    UP_RIGHT,
    UP,
    LEFT
};

struct Coord {
    Coord (int64_t i_val, int64_t j_val, Dir dir_val)
        : i(i_val)
        , j(j_val)
        , dir(dir_val)
    {
        assert(dir == DOWN_LEFT || dir == UP_RIGHT);
    }

    int64_t i;
    int64_t j;
    Dir dir;

    constexpr static int64_t di[4] = { 1, -1, -1,  0,};
    constexpr static int64_t dj[4] = {-1,  1,  0, -1,};

    Coord move(Dir dir) {
        Coord c = *this;

        c.i += di[dir];
        c.j += dj[dir];

        return c;
    }

    Coord move() {
        return this->move(dir);
    }

    Dir opposite() {
        switch(dir) {
            case Dir::UP:
                return Dir::LEFT;
            case Dir::LEFT:
                return Dir::UP;
            case Dir::DOWN_LEFT:
                return Dir::UP_RIGHT;
            case Dir::UP_RIGHT:
                return Dir::DOWN_LEFT;
            default:
                return Dir::DOWN_LEFT;
        };
    }

    friend std::ostream& operator << (std::ostream &o, const Coord& c)
    {
        return o << "(" << c.i << ", " << c.j << ")" << std::endl;
    }
};

int main()
{
    int64_t n;
    in >> n;

    for (int32_t i = 0; i < n; ++ i) {
        for (int32_t j = 0; j < n; ++ j) {
            in >> cost[i][j];
        }
    }

    dp[0][0][DIAG_DOWN] = cost[0][0];
    dp[0][0][DIAG_UP] = cost[0][0];

    auto get_value = [&](Coord coord) -> int64_t {
        int32_t i = coord.i;
        int32_t j = coord.j;

        if (i < 0 || j < 0 || i >= n || j >= n) {
            return MIN_VAL;
        }
        return dp[i][j][coord.dir];
    };

    auto get_best_profit = [&](Coord c) -> int64_t {
        return cost[c.i][c.j] + std::max(
                get_value(
                   c.move(c.opposite())
                ),
                std::max(get_value(c.move(LEFT)),
                         get_value(c.move(UP))
                )
        );
    };

    /*
        * * * 0 0 0
        * * 0 0 0 0
        * 0 0 0 0 0
        0 0 0 0 0 0
    */
    for (int64_t diagonal = 1; diagonal < n; ++ diagonal)
    {
        // Up-Right -> Down-Left
        Coord c = Coord (0, diagonal, Dir::DOWN_LEFT);

        while (c.j >= 0) {
            dp[c.i][c.j][c.dir] = get_best_profit(c);

            c = c.move();
        }

        // Down-Left -> Up-Right
        c = Coord (diagonal, 0, Dir::UP_RIGHT);
        while (c.i >= 0) {
            dp[c.i][c.j][c.dir] = get_best_profit(c);

            c = c.move();
        }

        // Compute final best
        c = Coord (diagonal, 0, Dir::UP_RIGHT);
        while (c.i >= 0) {
            int32_t i = c.i;
            int32_t j = c.j;
            dp[i][j][c.dir] = std::max(dp[i][j][c.opposite()], dp[i][j][c.dir]);
            dp[i][j][c.opposite()] = dp[i][j][c.dir];

            c = c.move();
        }
    }

    /*
        * * * * * *
        * * * * * *
        * * * * * 0
        * * * * 0 0
        * * * 0 0 0
        * * 0 0 0 0
    */
    for (int64_t diagonal = 1; diagonal < n; ++ diagonal) {
        // Up-Right -> Down-Left
        Coord c = Coord (diagonal, n - 1, Dir::DOWN_LEFT);

        while (c.i < n) {
            dp[c.i][c.j][c.dir] = get_best_profit(c);

            c = c.move();
        }

        // Down-Left -> Up-Right
        c = Coord (n - 1, diagonal, Dir::UP_RIGHT);
        while (c.j < n) {
            dp[c.i][c.j][c.dir] = get_best_profit(c);

            c = c.move();
        }

        // Compute final best
        c = Coord (n - 1, diagonal, Dir::UP_RIGHT);
        while (c.j < n) {
            int32_t i = c.i;
            int32_t j = c.j;
            dp[i][j][c.dir] = std::max(dp[i][j][c.opposite()], dp[i][j][c.dir]);
            dp[i][j][c.opposite()] = dp[i][j][c.dir];

            c = c.move();
        }
    }


    out << dp[n-1][n-1][DIAG_DOWN] << std::endl;
}
