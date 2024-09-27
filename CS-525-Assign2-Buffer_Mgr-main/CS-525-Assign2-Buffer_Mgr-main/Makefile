.PHONY: all
all: test_assign1 test_assign2

test_assign1: test_assign2_1.c storage_mgr.c dberror.c buffer_mgr.c buffer_mgr_stat.c replacement_mgr_strat.c buffer_mgr_helper.c
	gcc -g -o test_assign1 test_assign2_1.c storage_mgr.c dberror.c buffer_mgr.c buffer_mgr_stat.c replacement_mgr_strat.c buffer_mgr_helper.c

test_assign2: test_assign2_2.c storage_mgr.c dberror.c buffer_mgr.c buffer_mgr_stat.c replacement_mgr_strat.c buffer_mgr_helper.c
	gcc -g -o test_assign2 test_assign2_2.c storage_mgr.c dberror.c buffer_mgr.c buffer_mgr_stat.c replacement_mgr_strat.c buffer_mgr_helper.c

.PHONY: clean
clean:
	rm test_assign1 test_assign2 testbuffer.bin *.o
