#include <stdlib.h>
#include "blotto.h"

double blotto_play_game(size_t n, const int *a1, const int *a2, const double *v)
{
  if (n <= 0 || v == NULL || (a1 == NULL && a2 == NULL))
    {
      // no result (pitch unplayable); count as draw
      return 0.5;
    }
  else if (a1 == NULL)
    {
      // p1 wins by default
      return 0.0;
    }
  else if (a2 == NULL)
    {
      // p2 wins by default
      return 1.0;
    }
  
  double p1_score = 0.0;
  double p2_score = 0.0;
  for (size_t i = 0; i < n; i++)
    {
      if (a1[i] > a2[i])
	{
	  // p1 wins
	  p1_score += v[i];
	}
      else if (a1[i] < a2[i])
	{
	  // p2 wins
	  p2_score += v[i];
	}
      else
	{
	  // draw
	  p1_score += v[i] / 2;
	  p2_score += v[i] / 2;
	}
    }

  if (p1_score > p2_score)
    {
      return 1.0;
    }
  else if (p1_score < p2_score)
    {
      return 0.0;
    }
  else
    {
      return 0.5;
    }
}

	
