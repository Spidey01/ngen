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

#include "Statement.hpp"

Statement::Statement(const string& rule)
    : mRule(rule)
    , mInputs()
    , mOutputs()
    , mDependencies()
{
}


void Statement::appendInput(const string& input)
{
    mInputs.push_back(input);
}


void Statement::appendInputs(const list& inputs)
{
    mInputs.insert(mInputs.end(), inputs.begin(), inputs.end());
}


void Statement::appendOutput(const string& output)
{
    mOutputs.push_back(output);
}


void Statement::appendOutputs(const list& outputs)
{
    mOutputs.insert(mOutputs.end(), outputs.begin(), outputs.end());
}

std::ostream& operator<<(std::ostream& os, const Statement& stmt)
{
    using std::string;

    os << "build ";

    for (const string& output : stmt.mOutputs) {
        os << output << ' ';
    }

    os << ": " << stmt.mRule << " ";

    for (const string& input : stmt.mInputs) {
        os << input << ' ';
    }

    if (!stmt.mDependencies.empty()) {
        os << " | ";
    }
    for (const string& dep : stmt.mDependencies) {
        os << " " << dep;
    }

    os << std::endl;

    return os;
}
