#include <iostream>
#include "functions.h"

// returns the boolean value true
// if and only if 1,000 <= a <= b < 1,000,000,000,000
bool is_valid_range(int64_t a, int64_t b) {
    // TODO(student)
    if (1000LL <= a && a <= b && b < 1000000000000LL) return true;
    return false;
}

// returns 'p' if number is a plateau,
//         'b' if number is a basin,
//         'n' if number is neither
char classify_geo_type(int64_t number) {
    // TODO(student): Initialize local variables
    if (number < 0) return 'n';
    int64_t original_number = number;
    int digits = 0;
    int64_t t = number;
    while (t > 0) {
        ++digits;
        t /= 10;
    }
    if (digits < 4) return 'n';

    bool plateu_fail = false;
    int down_flat_up = 0;
    bool down = false;
    bool flat = false;
    bool up = false;
    int last_digit = 0;
    int second_last_digit = 0;

    // TODO(student): Determine whether number is a plateau
    //                This is the key step
    number = original_number;
    while (number >= 10) {
        last_digit = static_cast<int>(number % 10);
        second_last_digit = static_cast<int>((number / 10) % 10);

        if (down_flat_up == 0) {
            if (second_last_digit > last_digit) {
                down = true;
            } else if (second_last_digit == last_digit) {
                if (!down) { plateu_fail = true; break; }
                down_flat_up = 1;
                flat = true;
            } else {
                plateu_fail = true;
                break;
            }
        } else if (down_flat_up == 1) {
            if (second_last_digit == last_digit) {
                flat = true;
            } else if (second_last_digit < last_digit) {
                down_flat_up = 2;
                up = true;
            } else {
                plateu_fail = true;
                break;
            }
        } else {
            if (second_last_digit < last_digit) {
                up = true;
            } else {
                plateu_fail = true;
                break;
            }
        }

        number = number / 10;
    }

    if (!plateu_fail && down && flat && up) return 'p';

    // TODO(student): Determine whether number is a basin
    //                If you get the logic for plateau right,
    //                minor tweaks can create the code for basins.
    number = original_number;
    int up_flat_down = 0;
    down = false;
    flat = false;
    up = false;

    while (number >= 10) {
        last_digit = static_cast<int>(number % 10);
        second_last_digit = static_cast<int>((number / 10) % 10);

        if (up_flat_down == 0) {
            if (second_last_digit < last_digit) {
                up = true;
            } else if (second_last_digit == last_digit) {
                if (!up) return 'n';
                up_flat_down = 1;
                flat = true;
            } else {
                return 'n';
            }
        } else if (up_flat_down == 1) {
            if (second_last_digit == last_digit) {
                flat = true;
            } else if (second_last_digit > last_digit) {
                up_flat_down = 2;
                down = true;
            } else {
                return 'n';
            }
        } else {
            if (second_last_digit > last_digit) {
                down = true;
            } else {
                return 'n';
            }
        }

        number = number / 10;
    }

    if (down && flat && up) return 'b';

    // TODO(student): return the appropriate char:
    //                'p' for plateau,
    //                'b' for basin,
    //                'n' for neither
    return 'n';
    return 'x';
}

// returns how many numbers in the range [a, b] are plateaus and basins
plateaus_and_basins count_pb_numbers(int64_t a, int64_t b) {
    int number_of_plateaus = 0;
    int number_of_basins = 0;

    // TODO(student): count plateaus and basins in the range [a,b]
    struct Gen {
        static int digits(int64_t x) {
            if (x < 0) return 0;
            int c = 0;
            do {
                ++c;
                x /= 10;
            } while (x > 0);
            return c;
        }

        static bool append(int64_t prefix, int d, int64_t& out) {
            const int64_t LIM = 9223372036854775807LL;
            if (d < 0 || d > 9) return false;
            if (prefix > (LIM - d) / 10) return false;
            out = prefix * 10 + d;
            return true;
        }

        static void plateau_right(int max_d, int remaining, int64_t prefix,
                                  int64_t a, int64_t b, int64_t& count) {
            if (prefix > b) return;
            if (remaining == 0) {
                if (prefix >= a && prefix <= b) ++count;
                return;
            }
            for (int d = max_d; d >= 0; --d) {
                int64_t next;
                if (!append(prefix, d, next)) continue;
                plateau_right(d - 1, remaining - 1, next, a, b, count);
            }
        }

        static void plateau_left(int next_d, int max_d, int remaining,
                                 int64_t prefix, int p, int flat_len, int right_len,
                                 int64_t a, int64_t b, int64_t& count) {
            if (prefix > b) return;
            if (remaining == 0) {
                int64_t mid = prefix;
                for (int i = 0; i < flat_len; ++i) {
                    int64_t tmp;
                    if (!append(mid, p, tmp)) return;
                    mid = tmp;
                    if (mid > b) return;
                }
                plateau_right(p - 1, right_len, mid, a, b, count);
                return;
            }
            for (int d = next_d; d <= max_d; ++d) {
                int64_t next;
                if (!append(prefix, d, next)) continue;
                plateau_left(d + 1, max_d, remaining - 1, next, p, flat_len, right_len, a, b, count);
            }
        }

        static void basin_right(int next_d, int remaining, int64_t prefix,
                                int64_t a, int64_t b, int64_t& count) {
            if (prefix > b) return;
            if (remaining == 0) {
                if (prefix >= a && prefix <= b) ++count;
                return;
            }
            for (int d = next_d; d <= 9; ++d) {
                int64_t next;
                if (!append(prefix, d, next)) continue;
                basin_right(d + 1, remaining - 1, next, a, b, count);
            }
        }

        static void basin_left(int max_d, int min_d, int remaining, int64_t prefix,
                               int p, int flat_len, int right_len,
                               int64_t a, int64_t b, int64_t& count) {
            if (prefix > b) return;
            if (remaining == 0) {
                int64_t mid = prefix;
                for (int i = 0; i < flat_len; ++i) {
                    int64_t tmp;
                    if (!append(mid, p, tmp)) return;
                    mid = tmp;
                    if (mid > b) return;
                }
                basin_right(p + 1, right_len, mid, a, b, count);
                return;
            }
            for (int d = max_d; d >= min_d; --d) {
                int64_t next;
                if (!append(prefix, d, next)) continue;
                basin_left(d - 1, min_d, remaining - 1, next, p, flat_len, right_len, a, b, count);
            }
        }
    };

    if (a > b) return {0, 0};
    if (b < 1000) return {0, 0};
    if (a < 1000) a = 1000;

    int min_len = Gen::digits(a);
    if (min_len < 4) min_len = 4;
    int max_len = Gen::digits(b);
    if (max_len < 4) return {0, 0};

    int64_t plateauCount = 0;
    int64_t basinCount = 0;

    for (int n = min_len; n <= max_len; ++n) {
        for (int p = 2; p <= 9; ++p) {
            for (int flat_len = 2; flat_len <= n - 2; ++flat_len) {
                for (int left_len = 1; left_len <= n - flat_len - 1; ++left_len) {
                    int right_len = n - flat_len - left_len;
                    if (right_len < 1) continue;
                    if (left_len > (p - 1)) continue;
                    if (right_len > p) continue;
                    Gen::plateau_left(1, p - 1, left_len, 0, p, flat_len, right_len, a, b, plateauCount);
                }
            }
        }

        for (int p = 0; p <= 8; ++p) {
            int avail = 9 - p;
            for (int flat_len = 2; flat_len <= n - 2; ++flat_len) {
                for (int left_len = 1; left_len <= n - flat_len - 1; ++left_len) {
                    int right_len = n - flat_len - left_len;
                    if (right_len < 1) continue;
                    if (left_len > avail) continue;
                    if (right_len > avail) continue;
                    Gen::basin_left(9, p + 1, left_len, 0, p, flat_len, right_len, a, b, basinCount);
                }
            }
        }
    }

    number_of_plateaus = static_cast<int>(plateauCount);
    number_of_basins = static_cast<int>(basinCount);

    return {number_of_plateaus, number_of_basins};
}
