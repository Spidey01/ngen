#ifndef NGEN_EXTERNAL__HPP
#define NGEN_EXTERNAL__HPP
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

#include "Shinobi.hpp"
#include <utility>

/* Ninja generator - external backend.
 */
class external : public Shinobi
{
  public:

    external(Bundle& bundle);

    string generatorName() const override;

    bool generateBuildStatementsForExternal(const json& project, const string& type, const string& rule) override;
    bool generateBuildStatementsForTargetName(const json& project, const string& type, const string& rule) override;

  protected:

  private:
    string operatingSystemName() const;
    const json& operatingSystem() const;
};

#endif // NGEN_EXTERNAL__HPP

