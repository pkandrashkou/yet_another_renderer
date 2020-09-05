inline i32 abs_i32(i32 value) {
    return value < 0 ? -value : value;
}

inline void swap_i32(i32 *lhs, i32 *rhs) {
    i32 temp = *rhs;
    *rhs = *lhs;
    *lhs = temp;
}

global void interpolate_i32(i32 i0, i32 d0, i32 i1, i32 d1, i32 *result) {
    if (!result) {
        return;
    }

    if (i0 == i1) {
        result[0] = d0;
        return;
    }
    f32 a = (d1 - d0) / (f32)(i1 - i0);
    f32 d = (f32)d0;

    i32 index = 0;
    for (i32 i = i0; i <= i1; i++) {
        result[index] = (i32)d;
        index++;
        d = d + a;
    }
    return;
}

global void interpolate_f32(i32 i0, f32 d0, i32 i1, f32 d1, f32 *result) {
    if (!result) {
        return;
    }

    if (i0 == i1) {
        result[0] = d0;
        return;
    }
    f32 a = (d1 - d0) / (f32)(i1 - i0);
    f32 d = (f32)d0;

    i32 index = 0;
    for (i32 i = i0; i <= i1; i++) {
        result[index] = d;
        index++;
        d = d + a;
    }
    return;
}