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

    using json = nlohmann::json;

    using string = std::string;
    using list = std::vector<string>;

    using TypeToCompileRules = std::map<string, string>;
    using TypeToLinkRules = TypeToCompileRules;

    Shinobi(Bundle& bundle);

    /** Generate build.ninja by writing to mBundle.output.
     */
    virtual bool generate();

    /**
     */
    virtual bool generateProject(const json& project);

    /** Generate all the vars = values for the project.
     */
    virtual bool generateVariables(const json& project);

    /** Generate all the "rule ..." things for the project.
     */
    virtual bool generateRules();

    /** Generate all the "build object: rule source" for project.
     *
     * @param project reference to the project.
     * @param type the /project/type.
     * @param rule the compileRule(type).
     */
    virtual bool generateBuildStatementsForObjects(const json& project, const string& type, const string& rule);

    /** Generate the "build app: rule objects" for project.
     *
     * @param project reference to the project.
     * @param type the /project/type.
     * @param rule the linkRule(type).
     */
    virtual bool generateBuildStatementsForApplication(const json& project, const string& type, const string& rule);

    /** Generate the "build lib: rule objects" for project.
     *
     * @param project reference to the project.
     * @param type the /project/type.
     * @param rule the linkRule(type).
     */
    virtual bool generateBuildStatementsForLibrary(const json& project, const string& type, const string& rule);

    /** Generate the "build package: rule subprojects" for project.
     *
     * @param project reference to the project.
     * @param type the /project/type.
     * @param rule the ???(type).
     */
    virtual bool generateBuildStatementsForPackage(const json& project, const string& type, const string& rule);

    /** Returns the name of the backend.
     */
    virtual string generatorName() const;

    /** Explain your failure to disappointed master.
     */
    virtual void failure(std::ostream& log);

    std::ostream& log() const;

    /* Returns projectData().at("project") */
    string projectName() const;

    /* Returns projectData().at("type") */
    string projectType() const;

    /** Returns bundle().project.
     */
    const json& projectData() const;

    /** Returns true if obj[field] exists.
     */
    static bool has(const json& obj, const string& field);

  protected:

    const Bundle& bundle() const;

    bool debug() const;

    std::ostream& error() const;

    std::ostream& warning() const;

    std::ostream& output();

    /* Returns $sourcedir/source
     */
    string sourcedir(const string& source) const;

    /* Returns $builddir/source
     */
    string builddir(const string& source) const;

    /* Returns $distdir/source
     */
    string distdir(const string& source) const;

    /** Returns the rule name for compiling objects.
     */
    string compileRule(const string& type) const;

    /** Returns the rule name for linking objects.
     */
    string linkRule(const string& type) const;

  private:

    Bundle& mBundle;

    size_t mProjectIndex;

    /** Table of /project/type values to compile rule names.
     *
     * E.g. cxx_* -> c_compile; java_* -> java_compile; etc.
     */
    TypeToCompileRules mCompileRules;

    /** Table of /project/type values to link rule names.
     *
     */
    TypeToLinkRules mLinkRules;
};

#endif // NGEN_SHINOBI__HPP
