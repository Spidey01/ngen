#ifndef NGEN_STATEMENT__HPP
#define NGEN_STATEMENT__HPP
/*
 * Copyright 2019-current Terry Mathew Poulin <BigBoss1964@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

struct Bundle;

/** Ninja generator - base class.
 */
class Statement
{
  public:
    using string = std::string;
    using list = std::vector<std::string>;

    /** Creates an empty build statement.
     *
     * @param rule what rule to use.
     */
    Statement(const string& rule);

    Statement& appendInput(const string& input);
    Statement& appendInputs(const list& inputs);

    Statement& appendOutput(const string& output);
    Statement& appendOutputs(const list& outputs);

    Statement& appendDependency(const string& dep);
    Statement& appendDependencies(const list& deps);

    Statement& appendVariable(const string& name, const string& value);

    friend std::ostream& operator<<(std::ostream& os, const Statement& stmt);

  protected:

  private:

    string mRule;

    list mInputs;
    list mOutputs;
    list mDependencies;
    list mVariables;
};


std::ostream& operator<<(std::ostream& os, const Statement& stmt);

#endif // NGEN_STATEMENT__HPP
