#ifndef LIB_FLOATAQQ_COMPLEX
#define LIB_FLOATAQQ_COMPLEX


typedef struct {
  double real;
  double imag;
} complex_t;

double cmplx_mag (const complex_t a);
double cmplx_arg (const complex_t a);

complex_t cmplx_add (const complex_t a, const complex_t b);
complex_t cmplx_sub (const complex_t a, const complex_t b);
complex_t cmplx_mul (const complex_t a, const complex_t b);
complex_t cmplx_div (const complex_t a, const complex_t b);

complex_t cmplx_sqrt(const complex_t a);

complex_t cmplx_normalize_zero (const complex_t a);

bool cmplx_eq(const complex_t a, const complex_t b);

extern const complex_t CMPLX_I;

#endif // LIB_FLOATAQQ_COMPLEX
