#ifndef NGEN_SHINOBI__HPP
#define NGEN_SHINOBI__HPP
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

struct Bundle;

/** Ninja generator - base class.
 */
class Shinobi
{
  public:

    using unique_ptr = std::unique_ptr<Shinobi>;
    using shared_ptr = std::shared_ptr<Shinobi>;
    using weak_ptr = std::weak_ptr<Shinobi>;

    Shinobi(const Bundle& bundle);

    /** Generate build.ninja by writing to mBundle.output.
     */
    virtual bool generate();

    /** Explain your failure to disappointed master.
     */
    virtual void failure(std::ostream& log);

  protected:

    const Bundle& mBundle;

  private:
};

#endif // NGEN_SHINOBI__HPP
