#include <fstream>
#include <cstring>

#include "defination.h"

using namespace MIPS;

Status
Defination::collect()
{
  rapidxml::xml_node<char> *node;
  auto *root = doc.first_node();

  node = root->first_node("defination");
  if (!node)
    return ERROR_IN_SOURCE(S_INVALID_DEFINATION, "Invalid defination", "No defination found");
  for (auto *d = node->first_node(); d; d = d->next_sibling()) {
    defs[d->first_attribute("name")->value()] = d;
  }

  node = root->first_node("decleration");
  if (!node)
    return ERROR_IN_SOURCE(S_INVALID_DEFINATION, "Invalid defination", "No decleration found");
  for (auto *d = node->first_node(); d; d = d->next_sibling()) {
    decl[d->first_attribute("name")->value()] = d;
    sizes[d->first_attribute("name")->value()] = std::atoi(d->first_attribute("size")->value());
  }

  node = root->first_node("registers");
  if (!node)
    return ERROR_IN_SOURCE(S_INVALID_DEFINATION, "Invalid defination", "No registers found");
  for (auto *d = node->first_node("reg"); d; d = d->next_sibling("reg")) {
    registers[d->first_attribute("name")->value()] = std::atoi(d->first_attribute("id")->value());
  }

  return OK();
}

Status
Defination::load(const std::string &path)
{
  std::ifstream fin(path);

  if (!fin)
    return ERROR_IN_SOURCE(S_IO_FAILED, "Unable to open file", path);

  std::string buff( (std::istreambuf_iterator<char>(fin)),
                    (std::istreambuf_iterator<char>()));

  if (!buff.length())
    return ERROR_IN_SOURCE(S_IO_FAILED, "Empty content", path);

  content = new char [buff.length() + 1];

  if (!content)
    return ERROR_IN_SOURCE(S_MEM_FAILED, "Failed to allocate", "");

  std::strcpy(content, buff.c_str());

  doc.parse<0>(content);

  collect();

  return OK();
}
