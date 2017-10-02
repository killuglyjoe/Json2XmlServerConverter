#ifndef XML2JSON_HPP_INCLUDED
#define XML2JSON_HPP_INCLUDED

#include <iostream>
#include <sstream>
#include <string>
#include <cctype>
#include <map>

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_print.hpp"

#include "rapidjson/document.h"

namespace Json2Xml
{
    void  parseObject(const  rapidjson::Value& rootObj, rapidxml::xml_node<>* node, rapidxml::xml_document<> *xml_doc);

    void  appenNode(rapidxml::xml_node<> *rootNode, rapidxml::xml_document<> *xml_doc, const char *name)
    {
        rapidxml::xml_node<>* node = xml_doc->allocate_node(rapidxml::node_element, name);

        if(rootNode)
            rootNode->append_node(node);
        else
            xml_doc->append_node(node);
    }

    void  appenNode(rapidxml::xml_node<> *node, rapidxml::xml_node<> *rootNode, rapidxml::xml_document<> *xml_doc)
    {
        if(rootNode)
            rootNode->append_node(node);
        else
            xml_doc->append_node(node);
    }

    void  putAsNumber(const  rapidjson::Value& objName, rapidxml::xml_node<>* node, rapidxml::xml_document<> *xml_doc)
    {
        std::ostringstream strs;
        strs << objName.GetDouble();
        std::string str = strs.str();
        node->append_attribute(xml_doc->allocate_attribute(objName.GetString(), str.c_str()));
    }

    void  parseByObjectType(const  rapidjson::Value& objName,
                     rapidxml::xml_node<>* node, rapidxml::xml_document<> *xml_doc,
                     rapidjson::Value::ConstMemberIterator *rootIter)
    {
        switch (objName.GetType())
        {
            case rapidjson::kNullType:
                appenNode(node, xml_doc, "");
                break;

            case rapidjson::kFalseType:
                appenNode(node, xml_doc, "false");
                break;

            case rapidjson::kTrueType:
                appenNode(node, xml_doc, "true");
                break;

            case rapidjson::kObjectType:
                {
                    // this is root node
                    rapidxml::xml_node<>* elem = xml_doc->allocate_node(rapidxml::node_element, (*rootIter)->name.GetString());
                    // put all root attributes
                    parseObject(objName, elem, xml_doc);
                    // close root node
                    appenNode(elem, node, xml_doc);
                }
                break;

            case rapidjson::kArrayType:
                {
                    for (rapidjson::SizeType i = 0; i < objName.Size(); i++) // Uses SizeType instead of size_t
                    {
//                        std::cout << "--- Array " << objName[i].GetString() << std::endl;
                        if(objName[i].IsObject())
                            parseObject(objName[i], node, xml_doc);
                        else
                        {
                            parseByObjectType(objName[i], node, xml_doc, rootIter);
                        }
                    }
                }
                break;

            case rapidjson::kStringType:
                {
                    char *value = xml_doc->allocate_string((*rootIter)->value.IsString() ? (*rootIter)->value.GetString()
                                                                                         : objName.GetString());

                    rapidxml::xml_node<>* elem = xml_doc->allocate_node(rapidxml::node_element, (*rootIter)->name.GetString(),
                                                                        value);
                    appenNode(elem, node, xml_doc);
                }
                break;

            case rapidjson::kNumberType:
                {
                    std::string str("");
                    std::stringstream ss;

                    if(objName.IsDouble())
                        ss << (objName.GetDouble());

                    else if(objName.IsInt())
                        ss <<  (objName.GetInt());

                    else if(objName.IsUint())
                        ss <<  (objName.GetUint());

                    else if(objName.IsInt64())
                        ss <<  (objName.GetInt64());

                    else if(objName.IsUint64())
                        ss <<  (objName.GetUint64());

                    str = ss.str();
                    rapidxml::xml_node<>* elem = xml_doc->allocate_node(rapidxml::node_element, (*rootIter)->name.GetString(),
                                                                        xml_doc->allocate_string(str.data()));
                    appenNode(elem, node, xml_doc);
                }
                break;
            default:
                break;
        }
    }

    /**
     * @brief parse generic json object
     * @param objName
     * @param node
     * @param xml_doc
     */
    void  parseObject(const  rapidjson::Value& rootObj, rapidxml::xml_node<>* node, rapidxml::xml_document<> *xml_doc)
    {
        if(!rootObj.IsObject()) return;

        for( rapidjson::Value::ConstMemberIterator rootIter = rootObj.GetObject().MemberBegin();
              rootIter != rootObj.GetObject().MemberEnd(); ++rootIter)
        {
            const  rapidjson::Value &childObj = rootObj[rootIter->name.GetString()]; //get object value

            parseByObjectType(childObj, node, xml_doc,  &rootIter);

        }
    }

    std::string json2xml(const char *json_str)
    {
        //file<> fdoc("track_orig.xml"); // could serve another use case
        rapidxml::xml_document<> *xml_doc = new rapidxml::xml_document<>();

        rapidjson::Document js_doc;
        js_doc.Parse(json_str);


        if(js_doc.IsObject())
        {
            // xml declaration
            rapidxml::xml_node<>* decl = xml_doc->allocate_node(rapidxml::node_declaration);
            decl->append_attribute(xml_doc->allocate_attribute("version", "1.0"));
            decl->append_attribute(xml_doc->allocate_attribute("encoding", "utf-8"));
            xml_doc->append_node(decl);

            // this is root node
            rapidxml::xml_node<>* elem = xml_doc->allocate_node(rapidxml::node_element, xml_doc->allocate_string("documnet"));
            // put all root attributes
            parseObject(js_doc, elem, xml_doc);
            // close root node
            appenNode(elem, 0, xml_doc);
        }

        xml_doc->validate();

        // Convert doc to string
        std::string xml_as_string;
        rapidxml::print(std::back_inserter(xml_as_string), *xml_doc);

        delete xml_doc;

        return xml_as_string;
    }
}

#endif
