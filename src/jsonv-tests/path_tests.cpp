/** \file
 *  
 *  Copyright (c) 2014 by Travis Gockel. All rights reserved.
 *
 *  This program is free software: you can redistribute it and/or modify it under the terms of the Apache License
 *  as published by the Apache Software Foundation, either version 2 of the License, or (at your option) any later
 *  version.
 *
 *  \author Travis Gockel (travis@gockelhut.com)
**/
#include "test.hpp"

#include <jsonv/algorithm.hpp>
#include <jsonv/parse.hpp>
#include <jsonv/path.hpp>
#include <jsonv/value.hpp>

#include <fstream>

namespace jsonv_test
{

using namespace jsonv;

TEST(path_element_copy_compares)
{
    path_element elem1("hi");
    path_element elem2(1);
    path_element elem3(1);
    path_element elem4(elem1);
    path_element elem5(elem2);
    
    ensure_eq(elem1, elem1);
    ensure_ne(elem1, elem2);
    ensure_ne(elem1, elem3);
    ensure_eq(elem1, elem4);
    ensure_ne(elem1, elem5);
    
    ensure_ne(elem2, elem1);
    ensure_eq(elem2, elem2);
    ensure_eq(elem2, elem3);
    ensure_ne(elem2, elem4);
    ensure_eq(elem2, elem5);
    
    ensure_ne(elem3, elem1);
    ensure_eq(elem3, elem2);
    ensure_eq(elem3, elem3);
    ensure_ne(elem3, elem4);
    ensure_eq(elem3, elem5);
    
    ensure_eq(elem4, elem1);
    ensure_ne(elem4, elem2);
    ensure_ne(elem4, elem3);
    ensure_eq(elem4, elem4);
    ensure_ne(elem4, elem5);
    
    ensure_ne(elem5, elem1);
    ensure_eq(elem5, elem2);
    ensure_eq(elem5, elem3);
    ensure_ne(elem5, elem4);
    ensure_eq(elem5, elem5);
    
    elem3 = elem1;
    elem1 = elem2;
    
    ensure_eq(elem1, elem1);
    ensure_eq(elem1, elem2);
    ensure_ne(elem1, elem3);
    
    ensure_eq(elem2, elem1);
    ensure_eq(elem2, elem2);
    ensure_ne(elem2, elem3);
    
    ensure_ne(elem3, elem1);
    ensure_ne(elem3, elem2);
    ensure_eq(elem3, elem3);
}

TEST(path_concat_key)
{
    path p({ path_element("a") });
    path q = p + "b";
    ensure_eq(q, path({ path_element("a"), path_element("b") }));
    ensure_eq(to_string(q), ".a.b");
}

TEST(path_traverse)
{
    value tree;
    {
        std::ifstream stream("src/jsonv-tests/data/paths.json");
        tree = parse(stream);
    }
    
    traverse(tree,
             [this, &tree] (const path& p, const value& x)
             {
                 ensure_eq(to_string(p), x.as_string());
                 auto q = path::create(x.as_string());
                 ensure_eq(p, q);
                 ensure_eq(x, tree.at_path(p));
                 ensure_eq(x, tree.at_path(q));
             },
             true
            );
}

TEST(path_append_key)
{
    path p;
    p += "a";
    path q({ path_element("a") });
    ensure_eq(p, q);
}

TEST(path_create_simplestring)
{
    path p = path::create(".a.b.c");
    path q({ "a", "b", "c" });
    ensure_eq(p, q);
}

TEST(path_value_construction)
{
    value tree;
    tree.path(".a.b[2]") = "Hello!";
    tree.path(".a[\"b\"][3]") = 3;
    tree.path(".a[\"b\"][1]") = "Yo";
    
    value expected = object({
                             { "a", object({
                                            { "b", array({ nullptr, "Yo", "Hello!", 3 }) }
                                          })
                             }
                           });
    ensure_eq(expected, tree);
}

TEST(value_at_path_out_of_range)
{
    value tree;
    tree.path(".a.b[2]") = "Hello!";
    tree.path(".a[\"b\"][3]") = 3;
    tree.path(".a[\"b\"][1]") = "Yo";
    ensure_throws(std::out_of_range, tree.at_path(".does.not.exist"));
    const value& tree2 = tree;
    ensure_throws(std::out_of_range, tree2.at_path(".does.not.exist"));
}

TEST(value_path_array_construct)
{
    value arr;
    arr.path(0) = 0;
    arr.path(1) = 1;
    arr.path(2) = 2;
    ensure_eq(arr, array({ 0, 1, 2 }));
}

}
