int nums[6];

int explode(){};

void fun1(int *stacktop) // ai<=6
{
      int *e = stacktop;
      int g = stacktop;
      int f = stacktop;
      for (int a = 0; a != 6; ++a)
      {
            int current = *e;
            current--;
            if (current <= 5)
                  explode();
            e++;
      }
}

void fun2(int *stacktop) // ai=7-ai;
{
      int *g = stacktop + 6;
      int *f = stacktop;
      int *current = stacktop;
      int c;
      int d;
      do
      {
            c = d = 7;
            *current = 7 - *current;
            current++;
      } while (current != g)
}

void fun3(int *stacktop)
{
      int current;
      int g = 0;
      int *dx;
      int cx = 7;
      int bx;
      dx = 0x6032d0;

      do
      {
            :A
            *(stacktop + 2*g  + 20) = dx;//mov    %rdx,0x20(%rsp,%rsi,2)
            g += 4;

            if (g == 24)
            {
                  bx = *(stacktop + 32);
                  current = stacktop + 48;
                  g = stacktop + 80;
                  cx = bx;

                  dx = *current;
                  *(cx + 1) = dx;
                  current += 1;

                  while (current != g)
                  {
                        dx = *current;
                        *(cx + 1) = dx;
                        current += 1;
                        cx = dx;
                  }

                  int dp;

                  *(dx + 1) = 0;
                  dp = 5;

                  do
                  {
                        current = *(bx + 1);
                        if (*bx >= current)
                              explode();
                        bx = *(bx + 1);
                        bp--;
                  } while (*bx != current);
                  return;
            }

      } while (*(stacktop + 2 * g) <= 1);

      current = 1;
      dx = 0x6032d0;

      do
      {
            dx = *(dx + 1);
            current++;
      } while (cx != current);
      goto A;
}