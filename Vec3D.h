//A simple general vector class that teaches you the most important cool advanced features of C++
//ATTENTION it uses templates and operators
//This might be disturbing at first, but just think of T as either float or double
//both would be possible and you need no code changes.
//Here, we only use Vec3Df which is defined at the bottom as Vec3D<float>!

#pragma once

#include <cmath>
#include <iostream>

template<typename T> class Vec3D;



//operators allow you to write something like:
//Vec3Df t;
//Vec3Df t2;
//if (t1!=t2)
// and it calls the operator below

template <class T> bool operator!= (const Vec3D<T> & p1, const Vec3D<T> & p2) {
    return (p1[0] != p2[0] || p1[1] != p2[1] || p1[2] != p2[2]);
}

template <class T> Vec3D<T> operator* (const Vec3D<T> & p, float factor) {
    return Vec3D<T> (p[0] * factor, p[1] * factor, p[2] * factor);
}

template <class T> Vec3D<T> operator* (float factor, const Vec3D<T> & p) {
    return Vec3D<T> (p[0] * factor, p[1] * factor, p[2] * factor);
}

template <class T> Vec3D<T> operator* (const Vec3D<T> & p1, const Vec3D<T> & p2) {
    return Vec3D<T> (p1[0] * p2[0], p1[1] * p2[1], p1[2] * p2[2]);
}

template <class T> Vec3D<T> operator+ (const Vec3D<T> & p1, const Vec3D<T> & p2) {
    return Vec3D<T> (p1[0] + p2[0], p1[1] + p2[1], p1[2] + p2[2]);
}

template <class T> Vec3D<T> operator- (const Vec3D<T> & p1, const Vec3D<T> & p2) {
    return Vec3D<T> (p1[0] - p2[0], p1[1] - p2[1], p1[2] - p2[2]);
}

template <class T> Vec3D<T> operator- (const Vec3D<T> & p) {
    return Vec3D<T> (-p[0], -p[1], -p[2]);
}

template <class T> Vec3D<T> operator/ (const Vec3D<T> & p, float divisor) {
    return Vec3D<T> (p[0]/divisor, p[1]/divisor, p[2]/divisor);
}

template <class T> bool operator== (const Vec3D<T> & p1, const Vec3D<T> & p2) {
    return (p1[0] == p2[0] && p1[1] == p2[1] && p1[2] == p2[2]);
}

template <class T> bool operator< (const Vec3D<T> & a, const Vec3D<T> & b) {
    return (a[0] < b[0] && a[1] < b[1] && a[2] < b[2]);
}

template <class T> bool operator>= (const Vec3D<T> & a, const Vec3D<T> & b) {
    return (a[0] >= b[0] || a[1] >= b[1] || a[2] >= b[2]);
}




/**
 * Vector in 3 dimensions, with basics operators overloaded.
 */
template <typename T>
class Vec3D {
public:
    inline Vec3D (void)	{
        p[0] = p[1] = p[2] = T ();
    }
    inline Vec3D (T p0, T p1, T p2) {
        p[0] = p0;
        p[1] = p1;
        p[2] = p2;
    };
    inline Vec3D (const Vec3D & v) {
        init (v[0], v[1], v[2]);
    }
    inline Vec3D (T* pp) {
        p[0] = pp[0];
        p[1] = pp[1];
        p[2] = pp[2];
    };
    // ---------
    // Operators
    // ---------
    inline T& operator[] (int Index) {
        return (p[Index]);
    };
    inline const T& operator[] (int Index) const {
        return (p[Index]);
    };
    inline Vec3D& operator= (const Vec3D & P) {
        p[0] = P[0];
        p[1] = P[1];
        p[2] = P[2];
        return (*this);
    };
    inline Vec3D& operator+= (const Vec3D & P) {
        p[0] += P[0];
        p[1] += P[1];
        p[2] += P[2];
        return (*this);
    };
    inline Vec3D& operator-= (const Vec3D & P) {
        p[0] -= P[0];
        p[1] -= P[1];
        p[2] -= P[2];
        return (*this);
    };
    inline Vec3D& operator*= (const Vec3D & P) {
        p[0] *= P[0];
        p[1] *= P[1];
        p[2] *= P[2];
        return (*this);
    };
    inline Vec3D& operator*= (T s) {
        p[0] *= s;
        p[1] *= s;
        p[2] *= s;
        return (*this);
    };
    inline Vec3D& operator/= (const Vec3D & P) {
        p[0] /= P[0];
        p[1] /= P[1];
        p[2] /= P[2];
        return (*this);
    };
    inline Vec3D& operator/= (T s) {
        p[0] /= s;
        p[1] /= s;
        p[2] /= s;
        return (*this);
    };

    //---------------------------------------------------------------

    inline Vec3D & init (T x, T y, T z) {
        p[0] = x;
        p[1] = y;
        p[2] = z;
        return (*this);
    };
    inline T getSquaredLength() const {
        return (dotProduct (*this, *this));
    };
    inline T getLength() const {
        return (T)sqrt (getSquaredLength());
    };
    /// Return length after normalization
    inline T normalize (void) {
        T length = getLength();
        if (length == 0.0f)
            return 0;
        T rezLength = 1.0f / length;
        p[0] *= rezLength;
        p[1] *= rezLength;
        p[2] *= rezLength;
        return length;
    };
    inline Vec3D& unit (void) {
        T length = getLength();
        if (length != 0.0f)
		{
			T rezLength = 1.0f / length;
			p[0] *= rezLength;
			p[1] *= rezLength;
			p[2] *= rezLength;
		}
        return (*this);
    };
    static inline Vec3D crossProduct(const Vec3D & a, const Vec3D & b) {
        Vec3D result;
        result[0] = a[1] * b[2] - a[2] * b[1];
        result[1] = a[2] * b[0] - a[0] * b[2];
        result[2] = a[0] * b[1] - a[1] * b[0];
        return(result);
    }
    static inline T dotProduct(const Vec3D & a, const Vec3D & b) {
        return (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
    }
    static inline T distance (const Vec3D &v1, const Vec3D &v2) {
        Vec3D tmp = v1 - v2;
        return (tmp.getLength());
    }

protected:
    T p[3];
};

template <class T> std::ostream & operator<< (std::ostream & output, const Vec3D<T> & v) {
    output << v[0] << " " << v[1] << " " << v[2];
    return output;
}

template <class T> std::istream & operator>> (std::istream & input, Vec3D<T> & v) {
    input >> v[0] >> v[1] >> v[2];
    return input;
}

typedef Vec3D<float> Vec3Df;
