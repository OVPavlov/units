#ifndef UNITS_UNITS_H
#define UNITS_UNITS_H
#include <iostream>

// sec, kilogram, meter, Ampere, Kelvin, mole, Candela
template<int S /*seconds*/, int KG/*kilogram*/, int M/*meter*/, int A/*Ampere*/, int K/*Kelvin*/, int MOL/*mole*/, int CD/*Candela*/>
struct Dimension {
    static constexpr int S_val = S;
    static constexpr int KG_val = KG;
    static constexpr int M_val = M;
    static constexpr int A_val = A;
    static constexpr int K_val = K;
    static constexpr int MOL_val = MOL;
    static constexpr int CD_val = CD;
};

template<typename D1, typename D2>
struct DimensionAdd {
    using type = Dimension<
            D1::S_val + D2::S_val,
            D1::KG_val + D2::KG_val,
            D1::M_val + D2::M_val,
            D1::A_val + D2::A_val,
            D1::K_val + D2::K_val,
            D1::MOL_val + D2::MOL_val,
            D1::CD_val + D2::CD_val
    >;
};

template<typename D1, typename D2>
struct DimensionSubtract {
    using type = Dimension<
            D1::S_val - D2::S_val,
            D1::KG_val - D2::KG_val,
            D1::M_val - D2::M_val,
            D1::A_val - D2::A_val,
            D1::K_val - D2::K_val,
            D1::MOL_val - D2::MOL_val,
            D1::CD_val - D2::CD_val
    >;
};

template<typename T, typename Dim>
class Unit {
public:
    T value;

    constexpr Unit() : value(T{}) {}
    constexpr explicit Unit(T v) : value(v) {}

    template<typename D>
    constexpr auto operator*(const Unit<T, D>& rhs) const {
        using ResultDim = typename DimensionAdd<Dim, D>::type;
        return Unit<T, ResultDim>(value * rhs.value);
    }

    template<typename D>
    constexpr auto operator/(const Unit<T, D>& rhs) const {
        if constexpr (std::is_same_v<Dim, D>) {
            return value / rhs.value;
        } else {
            using ResultDim = typename DimensionSubtract<Dim, D>::type;
            return Unit<T, ResultDim>(value / rhs.value);
        }
    }

    constexpr auto operator*(float scalar) const {
        return Unit<T, Dim>(value * scalar);
    }

    constexpr auto operator/(float scalar) const {
        return Unit<T, Dim>(value / scalar);
    }
};


template <bool First, int EXP>
constexpr void Append(std::ostream& os, const char* name) {
    if constexpr (EXP != 0) {
        constexpr int expOffset = 6;
        constexpr const char* superscripts[] = {"⁻⁶","⁻⁵", "⁻⁴", "⁻³", "⁻²","⁻¹","", "", "²", "³", "⁴", "⁵", "⁶"};
        if constexpr (!First) os << "·";
        os << name;
        if constexpr (EXP != 1) {
            os << superscripts[EXP + expOffset];
        }
    }
}

template<typename T, typename Dim>
constexpr std::ostream& operator<<(std::ostream& os, const Unit<T, Dim>& u) {
    os << u.value << ' ';
    Append<true, Dim::KG_val>(os, "kg");
    constexpr bool firstM = (Dim::KG_val == 0);
    Append<firstM, Dim::M_val>(os, "m");
    constexpr bool firstS = firstM & (Dim::M_val == 0);
    Append<firstS, Dim::S_val>(os, "s");
    constexpr bool firstA = firstS & (Dim::S_val == 0);
    Append<firstA, Dim::A_val>(os, "A");
    constexpr bool firstK = firstA & (Dim::A_val == 0);
    Append<firstK, Dim::K_val>(os, "K");
    constexpr bool firstMOL = firstK & (Dim::K_val == 0);
    Append<firstMOL, Dim::MOL_val>(os, "mol");
    constexpr bool firstCD = firstMOL & (Dim::MOL_val == 0);
    Append<firstCD, Dim::CD_val>(os, "cd");
    return os;
}

template<typename T, typename Dim>
constexpr auto operator*(float scalar, const Unit<T, Dim>& unit) {
    return unit * scalar;
}

template<typename T, typename Dim>
constexpr auto operator/(float scalar, const Unit<T, Dim>& unit) {
    using InverseDim = Dimension<-Dim::S_val, -Dim::KG_val, -Dim::M_val, -Dim::A_val, -Dim::K_val, -Dim::MOL_val, -Dim::CD_val>;
    return Unit<T, InverseDim>(scalar / unit.value);
}


using s = Unit<float, Dimension<1, 0, 0, 0, 0, 0, 0>>;
using kg = Unit<float, Dimension<0, 1, 0, 0, 0, 0, 0>>;
using m = Unit<float, Dimension<0, 0, 1, 0, 0, 0, 0>>;
using A = Unit<float, Dimension<0, 0, 0, 1, 0, 0, 0>>;
using K = Unit<float, Dimension<0, 0, 0, 0, 1, 0, 0>>;
using mol = Unit<float, Dimension<0, 0, 0, 0, 0, 1, 0>>;
using cd = Unit<float, Dimension<0, 0, 0, 0, 0, 0, 1>>;

using mps = decltype(m() / s());
using mps2 = decltype(m() / (s() * s()));
using m2 = decltype(m() * m());
using m3 = decltype(m() * m() * m());
using Hz = decltype(float() / s());
using Newton = decltype(kg() * m() / (s() * s()));
using Joule = decltype(Newton() * m());
using Watt = decltype(Joule() / s());



constexpr s operator"" _s(long double v) { return s(v); }
constexpr kg operator"" _kg(long double v) { return kg(v); }
constexpr m operator"" _m(long double v) { return m(v); }
constexpr A operator"" _A(long double v) { return A(v); }
constexpr K operator"" _K(long double v) { return K(v); }
constexpr mol operator"" _mol(long double v) { return mol(v); }
constexpr cd operator"" _cd(long double v) { return cd(v); }


#endif //UNITS_UNITS_H
