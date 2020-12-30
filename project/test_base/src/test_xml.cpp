/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: test_aes.cpp
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#include "test_xml.h"
#include <string>
#include <iostream>
#include <stdio.h>

using namespace std;

test_xml::test_xml()
{

}

test_xml::~test_xml()
{

}

void test_xml::test()
{
    cout << "this is a xml string--------" << endl;
    pugi::xml_document config;
    std::string src = "<?xml version=\"1.0\" encoding=\"UTF - 8\" standalone=\"no\" ?><items id=\"123456\">100</items>"
        "<items id=\"123456\">100</items>";
    config.load_string(src.c_str());
    pugi::xml_node node = config.root().first_child();
    while (!node.empty()) {
        cout << node.text().as_string() << endl;
        cout << node.attribute("id").as_string() << endl;
        node = node.next_sibling();
    }

    cout << "this is a xml file--------" << endl;
    std::string position_config = "./config/xml_config.xml";
    pugi::xml_document doc;
    if (!doc.load_file(position_config.c_str())) {
        std::cout << "config file is not exist or invalid" << std::endl;
    }

    pugi::xml_node xroot = doc.child("position");
    if (xroot.empty()) {
        std::cout << "root element should be specified" << std::endl;
    }

    std::string tmp = xroot.child("date").text().as_string();
    cout << tmp << endl;

    tmp = xroot.child("sequenceno").text().as_string();
    cout << tmp << endl;

    xroot.child("date").text().set("2017-05-03");
    xroot.child("sequenceno").text().set(8889);
    doc.save_file(position_config.c_str());
}
