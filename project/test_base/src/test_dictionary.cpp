/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: test_dictionary.cpp
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#include "test_dictionary.h"
#include <stdio.h>

test_dictionay::test_dictionay()
{

}

test_dictionay::~test_dictionay()
{

}

void test_dictionay::test()
{
	base::dictionary* d = new base::dictionary;
	d->add_object("key1", false);
	d->add_object("key2", "value2");
	d->add_object("key3", (long)3);
	d->add_object("key4", 4.0f);

	base::dictionary* d1 = new base::dictionary;
	d1->add_object("key1", false);
	d1->add_object("key2", "value2");
	d1->add_object("key3", (long)3);

	base::darray* ar = new base::darray();
	ar->append("string");
	ar->append(true);
	ar->append((long)4);
	ar->append(5.0f);
	
	d->add_object("key5", ar);
	d->add_object("key6", d1);
	d->print();
    printf("----------------------------------------------------------\n");
	base::dictionary dcp(*d);
	dcp.print();
    printf("----------------------------------------------------------\n");
	base::dobject obj(*d);
	obj = dcp;
	obj.print();
    printf("----------------------------------------------------------\n");
	base::dobject obj2(*ar);
	obj2.print();
    printf("----------------------------------------------------------\n");
	obj2 = *ar;
	obj2.print();

    
    delete ar;
    delete d1;
	delete d;
}
