/* Copyright 2014-2018 Rsyn
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

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   PhysicalSpacingRule.h
 * Author: jucemar
 *
 * Created on 17 de Dezembro de 2017, 14:41
 */

#ifndef PHYSICALDESIGN_PHYSICALSPACINGRULE_H
#define PHYSICALDESIGN_PHYSICALSPACINGRULE_H

namespace Rsyn {

class PhysicalSpacingRule : public Proxy<PhysicalSpacingRuleData> {
        friend class PhysicalDesign;
        friend class PhysicalDesignData;
        RSYN_FRIEND_OF_GENERIC_LIST_COLLECTION;

       protected:
        //! @brief Constructs a Rsyn::PhysicalSpacingRule object with a pointer
        //! to Rsyn::PhysicalSpacingRuleData.
        PhysicalSpacingRule(PhysicalSpacingRuleData* data) : Proxy(data) {}

       public:
        //! @brief Constructs a Rsyn::PhysicalSpacingRule object with a null
        //! pointer to Rsyn::PhysicalSpacingRuleData.
        PhysicalSpacingRule() : Proxy(nullptr) {}
        //! @brief Constructs a Rsyn::PhysicalSpacingRule object with a null
        //! pointer to Rsyn::PhysicalSpacingRuleData.
        PhysicalSpacingRule(std::nullptr_t) : Proxy(nullptr) {}
        // @brief Returning default space
        DBU getSpacing() const;
        // @brief Returning end of line
        DBU getEndOfLine() const;
        // @brief Returning end of line within
        DBU getEndOfLineWithin() const;
};  // end class

}  // end namespace

#endif /* PHYSICALDESIGN_PHYSICALSPACINGRULE_H */
