// hist.cpp,v 1.3 2003/10/20 02:28:08 venkita Exp

/* Replacement for CSIM histogram created by Alex Carobus 7/20/98
   #include <std/disclaimer.h>
   */
/*
//
// Update history:
//
// [99.10.18] jcohen
//   version 1.3
//   added optional fields to header
//   to use: add_field("key","value",histogram);
//       or: add_field_n("key",45,histogram);
//
// [99.01.08] jcohen
//   version 1.2
//   histogram includes skew and kurtosis
//   call enable_skew(h) to enable skew and kurtosis on histogram h
//
// [98.07.29] jcohen
//   added add_histogram function
//   added version 1.1 display, added num_bins display
//   added read_hist
//
*/

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <string.h>
#include "ace/os_include/os_math.h"
#include "hist.h"

hist_t *head_hist, *tail_hist;

hist_t *histogram(char *name, unsigned int num_bins, double first,
                  double last) {
  hist_t *hist;
  if ((hist = (hist_t *)malloc(sizeof(hist_t))) == NULL) {
    fprintf(stderr, "unable to allocate memory for histogram : %s", name);
    exit(-1);
  }
  if ((hist->hs = (unsigned int *)malloc(sizeof(unsigned int) * (num_bins+2))) == NULL){
    fprintf(stderr, "unable to allocate memory for histogram : %s", name);
    exit(-1);
  }
  memset(hist->hs, 0, sizeof(unsigned int) * (num_bins+2));
  hist->name = name;
  hist->num_bins = num_bins;
  hist->first = first;
  hist->last = last;
  hist->num_points = 0;
  hist->max_num_outer = hist->num_outer = 0;
  hist->sum = 0.0;
  hist->sum2 = 0.0;
  hist->sum3 = 0.0;
  hist->sum4 = 0.0;
  hist->max = DBL_MIN;
  hist->min = DBL_MAX;
  hist->outer = NULL;
  hist->next = NULL;
  hist->skew = 0;
  hist->firstoptheader = NULL;
  if(head_hist == NULL)
    head_hist = tail_hist = hist;
  else {
    tail_hist->next = hist;
    tail_hist = tail_hist->next;
  }
  return hist;
}


void set_outer(unsigned int max_num_outer, hist_t *hist) {
  hist->outer = (double *)realloc(hist->outer,sizeof(double) * max_num_outer);
  hist->max_num_outer = max_num_outer;
}


void enable_skew(hist_t *hist) {
  hist->skew = 1;
}


void add_field(char *key, char *value, hist_t *hist) {
  struct optheader *nextoptheader, *trace;

     /* create and prepare nextoptheader */
  nextoptheader = (struct optheader *) malloc(sizeof(struct optheader));
  nextoptheader->key = (char *) malloc(strlen(key)+1);
  nextoptheader->value = (char *) malloc(strlen(value)+1);
  nextoptheader->next = NULL;
  strcpy(nextoptheader->key,key);
  strcpy(nextoptheader->value,value);

     /* tack nextoptheader onto end of optheader list */
  if (hist->firstoptheader == NULL) {
     hist->firstoptheader = nextoptheader;
  } else {
     trace = hist->firstoptheader;
     while (trace->next != NULL) {
        trace = trace->next;
     }
     trace->next = nextoptheader;
  }
}


void add_field_n(char *key, int value, hist_t *hist) {
  char textvalue[40];

  sprintf(textvalue,"%d",value);
  add_field(key,textvalue,hist);
}


void record(double point, hist_t *hist) {
  unsigned int bin;
  double incsum;
  if (point < hist->min) hist->min = point;
  if (point > hist->max) hist->max = point;
  if (point < hist->first) {
    bin = 0;
    if (hist->max_num_outer > hist->num_outer)
      hist->outer[hist->num_outer++] = point;
  }
  else if (point >= hist->last) {
    bin = hist->num_bins+1;
    if (hist->max_num_outer > hist->num_outer)
      hist->outer[hist->num_outer++] = point;
  }
  else
    bin = (unsigned int)(((point - hist->first)*(hist->num_bins))/(hist->last - hist->first)+1);

  hist->hs[bin]++;
  hist->num_points++;
  hist->sum+=point;
  incsum = point*point;
  hist->sum2 += incsum;
  if (hist->skew) {
    incsum *= point;
    hist->sum3 += incsum;
    incsum *= point;
    hist->sum4 += incsum;
  }

/*  printf("point : %g, bin %u, points in bin %u\n", point, bin, hist->hs[bin]); */
}

void report_to(FILE *strm, hist_t *hist) {
  unsigned int sofar, i;
  double p1, p2, d, mean;
  double variance;
  struct optheader *trace;

  if (hist->num_points == 0) {
    fprintf(strm, "\n\n\t\t\t\tHistogram %s is empty\n\n", hist->name);
    return;
  }
  fprintf(strm, "\n\n\t\t\t\tHistogram %s\n", hist->name);
  if (hist->skew) fprintf(strm, "version: %s\n", HIST_VERSION);
  else fprintf(strm, "version: 1.1\n");
  fprintf(strm, "minimum: %g\n", hist->min);
  fprintf(strm, "maximum: %g\n", hist->max);
  fprintf(strm, "mean: %g\n", mean = hist->sum/hist->num_points);
  variance = (hist->sum2 -
          2*hist->sum*mean +
          hist->num_points*mean*mean) / (hist->num_points-1);
  fprintf(strm, "variance: %g\n", variance);
  if (hist->skew) {
    fprintf(strm, "skew: %g\n", ((hist->sum3 -
          3*hist->sum2*mean +
          3*hist->sum*mean*mean -
          hist->num_points*mean*mean*mean) /
          pow(sqrt(variance),3) / hist->num_points));
    fprintf(strm, "kurtosis: %g\n", ((hist->sum4 -
          4*hist->sum3*mean +
          6*hist->sum2*mean*mean -
          4*hist->sum*mean*mean*mean +
          hist->num_points*mean*mean*mean*mean) /
          pow(sqrt(variance),4)) / hist->num_points - 3);
  }
  fprintf(strm, "num_points: %u\n", hist->num_points);
  fprintf(strm, "num_bins: %d %g %g\n", hist->num_bins,hist->first,hist->last);
  if (hist->firstoptheader) {
     trace = hist->firstoptheader;
     while(trace->next != NULL) {
        fprintf(strm, "%s: %s\n", trace->key, trace->value);
        trace = trace->next;
     }
     fprintf(strm, "%s: %s\n", trace->key, trace->value);
  }
  fprintf(strm, "\n");
  sofar = hist->hs[0];
  fprintf(strm, "\t      Low     -          High     Count  Fraction  Cumulative\n");
  fprintf(strm, "\t     below    -  %12.3f  :  %5u    %0.3f    %0.3f\n", hist->first,
          hist->hs[0], (double)hist->hs[0]/hist->num_points,
          (double)histfloor(1000.0*sofar/hist->num_points)/1000.0);
  p2 = hist->first;
  d = (hist->last - hist->first)/hist->num_bins;
  for(i = 1; i <= hist->num_bins; i++) {
    p1 = p2;
    p2 = i*d+hist->first;
    sofar += hist->hs[i];
    fprintf(strm, "\t%12.3f  -  %12.3f  :  %5u    %0.3f    %0.3f\n", p1, p2, hist->hs[i],
            (double)hist->hs[i]/hist->num_points,
            (double)histfloor(1000.0*sofar/hist->num_points)/1000.0);
  }
  sofar += hist->hs[hist->num_bins+1];
  fprintf(strm, "\t%12.3f  -       above    :  %5u    %0.3f    %0.3f\n\n",
          hist->last, hist->hs[hist->num_bins+1],
          (double)hist->hs[hist->num_bins+1]/hist->num_points,
          (double)histfloor(1000.0*sofar/hist->num_points)/1000.0);
  if (hist->num_outer) {
    fprintf(strm, "outliers:\n");
    for(i = 0; i < hist->num_outer; i++) fprintf(strm, "\t%12.3f\n",
                                            hist->outer[i]);
  }
  fprintf(strm, "\n\n");
}

void report() {
  hist_t *temp = head_hist;
  for(; temp; temp = temp->next)
    report_to(stdout, temp);
}

void stats_init() {
  /* this is not necessary */
}

double get_mean(HIST hist) {
  return(hist->sum/hist->num_points);
}

double get_min(HIST hist) {
  return(hist->min);
}

double get_max(HIST hist) {
  return(hist->max);
}

double get_variance(HIST hist) {
  double mean = hist->sum/hist->num_points;
  return((hist->sum2 - 2*hist->sum*mean + hist->num_points*mean*mean)/
    (hist->num_points-1));
}

double get_num(HIST hist) {
  return(hist->num_points);
}

void set_mean(HIST hist, double mean) {
  hist->sum = mean*hist->num_points;
}

void set_min(HIST hist, double min) {
 hist->min = min;
}

void set_max(HIST hist, double max) {
  hist->max = max;
}

void set_variance(HIST hist, double variance) {
  /* do this after seting mean */
  double mean = hist->sum/hist->num_points;
  hist->sum2 = variance *(hist->num_points-1) + 2*hist->sum*mean - hist->num_points*mean*mean;
}

void add_histogram(HIST dest, HIST source) {
  unsigned int i,j;
  double d,midpoint;

   d = (source->last - source->first)/source->num_bins;

   for(i=1;i<=source->num_bins;i++) {
      midpoint = source->first + ((i - 0.5) * d);
      for(j=0;j<source->hs[i];j++) {
         record(midpoint,dest);
      }
   }

   for(i=0;i<source->num_outer;i++)
      record(source->outer[i],dest);

   set_min(dest, get_min(source));
   set_max(dest, get_max(source));
   set_mean(dest, get_mean(source));
   set_variance(dest, get_variance(source));
}

double histfloor(double x) {

#ifdef WIN32
  return floor(x);
#else
  return((long long)x);
#endif

}
