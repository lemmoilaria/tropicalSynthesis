/**
 @file
 tropicaladd~ - a tropical additive synthetizer
 */

/*
 formula;
 trop(x[n])= min{a1 +cos(ω1n+φ1),a2 + cos(ω2n+φ2),...,ap +cos(ωpn+φp)}
 */


#include "stdlib.h"
#include "time.h"
#include "ext.h"
#include "z_dsp.h"
#include "ext_obex.h"
#include "math.h"

#define MAX_OSC 100

typedef struct _tropicaladd {
    t_pxobject t_obj;
    double freq;
    long n_osc;
    float dist_spec; // **
    double amp[MAX_OSC]; // ***
    double phase[MAX_OSC];
    double sr;
    double ym1; // per filtro dc offset, campione precedente y
    double ym2; // per filtro dc offset, 2 campione precedente y
    double xm1; // per filtro dc offset, campione precedente x
    double xm2; // per filtro dc offset, 2 campione precedente x
} t_tropicaladd;


double minimum_samples(double *array, int grandezza) ;
void interp_lin(double y0, double y1, double* array);
float* linearInterpolation(float* src, int src_len, int steps, float* dst);

void myobject_float(t_tropicaladd *x, double freq);
void myobject_in1(t_tropicaladd *x, long num_osc);
void myobject_ft2(t_tropicaladd *x, double dist_spettrale);
void myobject_ft3(t_tropicaladd *x, double amp);
void myobject_ft4(t_tropicaladd *x, double phase);
void tropicaladd_free(t_tropicaladd *x);
void tropicaladd_assist(t_tropicaladd *x, void *b, long m, long a, char *s);
void *tropicaladd_new(t_symbol *s,  long argc, t_atom *argv);
void tropicaladd_dsp64(t_tropicaladd *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
void tropicaladd_perform64(t_tropicaladd *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam);



/**************************************************************************************************************************/


static t_class *s_tropicaladd_class;


void ext_main(void *r)
{
    t_class *c = class_new("tropicaladd~", (method)tropicaladd_new, (method)tropicaladd_free, sizeof(t_tropicaladd), NULL, A_GIMME, 0);
    
    class_addmethod(c, (method)tropicaladd_dsp64,        "dsp64",    A_CANT, 0);
    class_addmethod(c, (method)tropicaladd_assist,        "assist",    A_CANT, 0);
    
    class_addmethod(c, (method)myobject_float, "float", A_FLOAT, 0);
    class_addmethod(c, (method)myobject_in1, "in1", A_LONG, 0);
    class_addmethod(c, (method)myobject_ft2, "ft2", A_FLOAT, 0);
    class_addmethod(c, (method)myobject_ft3, "ft3", A_FLOAT, 0);
    class_addmethod(c, (method)myobject_ft4, "ft4", A_FLOAT, 0);

    class_dspinit(c);
    
    class_register(CLASS_BOX, c);
    s_tropicaladd_class = c;
    
    post("Tropical Additive Synthetizer loaded.");
}

void *tropicaladd_new(t_symbol *s,  long argc, t_atom *argv)
{
    t_tropicaladd *x = (t_tropicaladd *)object_alloc(s_tropicaladd_class);
    
    dsp_setup((t_pxobject *)x,0);

    floatin(x, 4);
    floatin(x, 3);
    floatin(x, 2);
    intin(x, 1);
    
    outlet_new((t_object *)x, "signal");
    
    return (x);
}


void tropicaladd_free(t_tropicaladd *x)
{
    dsp_free((t_pxobject *)x);
}


void tropicaladd_assist(t_tropicaladd *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET) {
        switch (a) {
            case 0:    snprintf_zero(s, 256, "(float) Fondamental Frequency)");    break;
            case 1:    snprintf_zero(s, 256, "(int) Number of oscillators");     break;
            case 2:    snprintf_zero(s, 256, "(float) Spectral distortion");    break;
            case 3:    snprintf_zero(s, 256, "(float) Amplitude");    break;
            case 4:    snprintf_zero(s, 256, "(float) Phase");    break;
        }
    }
    else {
        snprintf_zero(s, 256, "(signal) Output %ld", a+1);
    }
}



void myobject_float(t_tropicaladd *x, double freq) {
    x->freq = freq;
}

void myobject_in1(t_tropicaladd *x, long num_osc){
    if (num_osc < MAX_OSC) {
        x->n_osc = num_osc;
    } else if (num_osc >= MAX_OSC) {
        num_osc = MAX_OSC;
        x->n_osc = num_osc;
    }
}


void myobject_ft2(t_tropicaladd *x, double dist_spettrale) {
    x->dist_spec = dist_spettrale;
}


void myobject_ft3(t_tropicaladd *x, double amp) {
        for (int i = 0; i < MAX_OSC; i++) {
            if (amp > 0.) {
                x->amp[i] = amp;
            } else if (amp <= 0.) {
            x->amp[i] = rand()%10/10.0;
            }
        }
}

void myobject_ft4(t_tropicaladd *x, double phase) {
    
    for (int i = 0; i < MAX_OSC; i++) {
        x->phase[i] = phase;
    }
}


void tropicaladd_perform64(t_tropicaladd *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam)
{
    t_double        *out = outs[0];
    
    double freq = x->freq;
    double dist = x->dist_spec;
    int n_osc = x->n_osc;
    double* amp = x->amp;
    double* phase = x->phase;
    double min, tempx, tempy;
    double freqArray[n_osc]; // in windows deve avere una grandezza fissa
    int n;
    double y1 = x->ym1;
    double y2 = x->ym2;
    double x1 = x->xm1;
    double x2 = x->xm2;
    double y0;
    
    int addendo[10];

    for (int i = 0; i < 10; i++) {
        int x = i+1;
        addendo[i] = x*2;
    }
    
    
    for (int i = 0; i < n_osc; i++) {
        if (dist > 0.) {
            freqArray[i] = pow(freq * (i+1), dist);
        } else if (dist <= 0.) {
            int distx = (int) (fabs(dist));
            int numero = (addendo[(distx-1)]) * 2;
            double randomN = (rand()%(numero));
            freqArray[i] = (randomN - (numero/2)) + freq;
        }
    }
    
    
    for (n = 0; n < sampleframes; n++) {
        double segnale[n_osc];
        double incr[n_osc];
        tempx = x1;
        tempy = y1;
        for (int i = 0; i < n_osc;i++) {
       
            segnale[i] = amp[i] + cos(phase[i]);
            incr[i] = freqArray[i] * TWOPI / x->sr;
            phase[i] += incr[i];
            
        }
        
        double min = minimum_samples(segnale, n_osc);
        //  0.999644 -1.999288 0.999644 -1.999287 0.999288 --> i due segni di y1 e y2 sono da invertire rispetto ai valori di max
        // y(n) = a0x(n)+a1x(n-1)+a2x(n-2)-b1y(n-1)-b2y(n-2)

        y0 = (0.999644 * min) + (-1.999288 *(x1)) + (0.999644*(x2)) - (1.999287*(y1)) - (-0.999288*(y2));
        x2 = tempx;
        y2 = tempy;
        out[n] = y0 ;

     }
    
    x->ym1 = y1;
    x->ym2 = y2;
    x->xm1 = x1;
    x->xm2 = x2;

}

void tropicaladd_dsp64(t_tropicaladd *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    srand((unsigned int) time(NULL)) ;
    object_method(dsp64, gensym("dsp_add64"), x, tropicaladd_perform64, 0, NULL);
    
    x -> sr = samplerate;
    myobject_ft4(x, 0.);
}



double minimum_samples(double *array, int grandezza) {
    double minimo = array[0];
    for (int i = 1; i < grandezza; i++) {
        if (array[i] < minimo) {
            minimo = array[i];
        }
    }
    return (minimo);
}
