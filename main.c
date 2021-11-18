#include <stdio.h>
#include "linux/pbit.h"
#include <errno.h>
#include <assert.h>
/*
 * struct pbit {
 int rs; 0-3
 int ev1; 4-7
 int status; 8-11
 int ev2; 12-15
 int ms; 16-19
 int ev3; 20-23
 int dead; 24-27
 int ls; 29-31
 }	;
 */

int main(int argc, char *argv[]) {
	struct pbit pc;
	int i, j, swp, *p;
	int num_hits, num_fails;


	pc.rs = 1; //0
	pc.ev1 = 2; //1
	pc.status = 3; //2
	pc.ev2 = 4; //3
	pc.ms = 5; //4
	pc.ev3 = 6; //5
	pc.dead = 7; //6
	pc.ls = 8; //7

	for (i = 0; i < sizeof(struct pbit) / 4; i++)
	{
		p = (int *)&pc;
		p += i;
		printf("XXX %d:%d\n", i, *p);
	}
	printf("ZZZ:{%d,%d,%d,%d,%d,%d,%d,%d}\n", pc.rs, pc.ev1, pc.status, pc.ev2,
		   pc.ms, pc.ev3, pc.dead, pc.ls);

	pbit_check_setup(&pc, 65535);
	pbit_check_yes(&pc, 65535);
	num_hits = 0;
	num_fails = 0;
	setbuf(stdout, NULL);

	printf("Size:%d\n", sizeof(struct pbit));

	// BAAAAAAA - 17 fo, 15 fi


	for (i = 0; i < sizeof(struct pbit) / 4; i++) {
	//for (i = 2; i < 3; i++) {
		p = (int*) &pc;
		p += i;
		if (pbit_check(&pc) != PBIT_YES) {
			switch (i) {
			case 1:
			case 3:
			case 5:
				break;
			default:
				printf("FOOK%d\n",i);
				num_fails++;
				break;
			}

		}
		for (j = 0; j < (sizeof(int) * 8); j++) {
			p = (int*) &pc;
			p += i;
			swp = *p;
			*p = (*p) & (1 << j);
			if (*p != swp) {
				num_hits++;
				if (pbit_check(&pc) == PBIT_YES) {
					switch (i) {
					case 1:
					case 3:
					case 5:
						if (pbit_infer(&pc) != -EINVAL)
							num_fails++;
						break;
					default:
						printf("A%d\n",i);
						num_fails++;
						break;
					}
				}
			}
			*p = swp;
			*p = (*p) ^ (1 << j);
			if (*p != swp) {
				num_hits++;
				if (pbit_check(&pc) == PBIT_YES) {
					switch (i) {
					case 1:
					case 3:
					case 5:
						if (pbit_infer(&pc) != -EINVAL)
							num_fails++;
						break;
					default:
						printf("B%d\n",i);
						num_fails++;
						break;
					}
				}
			}
			*p = swp;
		}
	}

	if (pbit_check(&pc) != PBIT_YES)
		num_fails++;

	pbit_check_no(&pc, 65535);

	for (i = 0; i < sizeof(struct pbit) / 4; i++) {
	//for (i = 2; i < 3; i++) {
		p = (int*) &pc;
		p += i;
		if (pbit_check(&pc) != PBIT_NO) {
			switch (i) {
			case 1:
			case 3:
			case 5:
				if (pbit_infer(&pc) != -EINVAL)
					num_fails++;
				break;
			default:
				printf("HELL%d\n",i);
				num_fails++;
				break;
			}
		}
		for (j = 0; j < (sizeof(int) * 8); j++) {
			p = (int*) &pc;
			p += i;
			swp = *p;
			*p = (*p) & (1 << j);
			if (*p != swp) {
				num_hits++;
				if (pbit_check(&pc) == PBIT_NO) {
					switch (i) {
					case 1:
					case 3:
					case 5:
						if (pbit_infer(&pc) != -EINVAL)
							num_fails++;
						break;
					default:
						printf("C%d\n",i);
						num_fails++;
						break;
					}
				}
			}
			*p = swp;
			*p = (*p) ^ (1 << j);
			if (*p != swp) {
				num_hits++;
				if (pbit_check(&pc) == PBIT_NO) {
					switch (i) {
					case 1:
					case 3:
					case 5:
						if (pbit_infer(&pc) != -EINVAL)
							num_fails++;
						break;
					default:
						printf("D%d\n",i);
						num_fails++;
						break;
					}
				}
			}
			*p = swp;
		}
	}

	if (pbit_check(&pc) != PBIT_NO)
		num_fails++;

	assert(num_fails == 0);
	assert(num_hits == 1024);
	//printf("Num Hits:%d\n", num_hits);
	//printf("Num Fail:%d\n", num_fails);

	PBIT_Y(pc,1);
	assert(pbit_check(&pc) == PBIT_YES);
	assert(PBIT_GET(pc) == 1);
	//printf("PBV:%d,%d\n", pbit_check(&pc), PBIT_GET(pc));
	PBIT_N(pc,2);
	assert(pbit_check(&pc) == PBIT_NO);
	assert(PBIT_GET(pc) == 2);
	//printf("PBV:%d,%d\n", pbit_check(&pc), PBIT_GET(pc));
	//printf("PBS:%d\n", PBIT_OK(pc));
	//printf("PBS:%d\n", PBIT_FAIL(pc));
	assert(PBIT_OK(pc) == 0);
	assert(PBIT_FAIL(pc) == 1);

	PBIT_Y(pc,100);
	assert(PBIT_OK(pc) == 1);
	assert(PBIT_FAIL(pc) == 0);

	pbit_check_setup(&pc, 0);

	assert(PBIT_OK(pc) == 0);
	assert(PBIT_FAIL(pc) == 0);
	assert(PBIT_DEAD(pc) == 1);

	assert(pbit_check(&pc) == PBIT_ERR);
	//printf("PBS:%d\n", PBIT_OK(pc));
	//printf("PBS:%d\n", PBIT_FAIL(pc));
	PBIT_Y(pc,100);
	PBIT_RET(pc);
	//printf("X\n");
}
