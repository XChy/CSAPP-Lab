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
      int esi = 0;
      int ecx = 7;
      int *edx;
      int eax;

      ecx = stacktop[esi]; // 401197
      // 7-a1
      if (ecx <= 1)
      { // 401183
            edx = 0x6032d0;
            stacktop[esi * 2 + 32] = 0x6032d0; // stacktop[32]=0x6032d0
            esi = 4;
            ecx = stacktop[esi]; // ecx=7-a2
      }
      do
      {
            eax = 1; // 40119f
            edx = 0x6032d0;

            do // 401176
            {
                  edx = *(edx + 8);
                  eax += 1;
            } while (ecx != eax);
            // 401188
            do
            {
                  // edx = 0x6032d0;
                  stacktop[esi * 2 + 32] = edx; // stacktop[32]
                  esi += 4;
                  if (esi == 24)
                        break;
                  ecx = stacktop[esi];
            } while (ecx <= 1);
      } while (1);

      int ebx; // 4011ab
      ebx = stacktop[32];
      eax = stacktop + 40;
      esi = stacktop + 80;
      ecx = ebx;
      while (1)
      {
            // 4011bd
            edx = *(int *)eax;
            *(int *)(ecx + 8) = edx;
            eax += 8;
            if (eax == esi)
                  break;
            ecx = edx;
      }

      int ebp;
      *(edx + 8) = 0;
      ebp = 5;
      // 4011df
      int current;
      do
      {
            eax = *(int *)(ebx + 8);

            current = *(int *)ebx;//单调递减
            if (eax < *(int *)ebx)
                  explode();
            ebx = *(int *)(ebx + 8);
            ebp--;
      } while (eax != current);
}