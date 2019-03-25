/**
	Copyright 2014-2019 Phantom Intelligence Inc.

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

#ifndef SENSORGATEWAY_CALLBACKTUPLETYPE_H
#define SENSORGATEWAY_CALLBACKTUPLETYPE_H

namespace {

    // TODO: Rename and generalize this for the:
    //      - staticly declared map it is
    //      - knowing it can be searched by StringLiteral
    //      - and filled / populated in the same fashion a ServiceLocator is
    //      - except this can be populated by *anything*
    //      - specifically here though, it is populated by callbacks
    //      - it could become something like :
    //      <type... (of which instances will be stored)>::
    //      <descriptors (any structure, typedef, template, etc... useful for the stored type)>::
    //      <name... (used to locate the instance or descriptors once populated)>

    /**
     * // TODO: explore std::ignore about that
     * Since we cannot have *no* typename in Descriptions
     * but some callbacks might not take in any parameters, this helps lookup and definition
     */
    struct EmptyDescription {
    };

    // NOTE : ***At the moment*** this structure is only used to store and locate callbacks (--> CallBackLocator). Important to note that it has much more potential
    /**
     * This structure allows to simplify handling of callback with similar signature and usage.
     * Please note that the type description below are suggestion based on the first meant usage of this structure.
     * @note At the moment, this structure is used as InstanceLocator, so in this case, [InstanceType ==> std::function<void(Description)>]
     * @tparam InstanceTypes a series of type of which one instance of each will be stored in the structure
     */
    template<typename ...InstanceTypes>
    struct CallBack {

        /**
         * @note At the moment, this structure is used as InstanceLocator, so in this case, [Description ==> ArgumentType (as in std::function<void(ArgumentType)>)]
         * @tparam Descriptions a series of types which describe/help the usage of the InstanceType they are associated with
         */
        template<typename ...Descriptions>
        struct UsingArgument {

            /**
             * Allows to find the instance by name.
             * @tparam Names StringLiteral describing the instance type
             */
            template<typename ...Names>
            struct Named {

                static_assert(sizeof...(InstanceTypes) == sizeof...(Descriptions) &&
                              sizeof...(InstanceTypes) == sizeof...(Names),
                              "\n\nThe number of instances, descriptions and names must be the same in order to create CallBacksTuple!\n\n");
                static constexpr size_t const size = sizeof...(InstanceTypes);

                // WARNING! CHANGING THE ORDER OF THESE ELEMENTS WILL IMPACT THE TYPE HELPERS, PLEASE ENSURE THOSE ARE STILL VALID
                using AssociationTuples = std::tuple<std::tuple<InstanceTypes, Descriptions, Names>...>;

                using InstancesTuple = std::tuple<InstanceTypes...>;
                using DescriptionTuple = std::tuple<Descriptions...>;
                using NamesTuple = std::tuple<Names...>;

                template<typename T, typename List>
                using AssociationFrom = typename std::tuple_element_t<IndexOf<T, List>::value, AssociationTuples>;

                template<typename F>
                using AssociationFromInstanceType = AssociationFrom<F, InstancesTuple>;

                template<typename A>
                using AssociationFromDescription = AssociationFrom<A, DescriptionTuple>;

                template<typename N>
                using AssociationFromName = AssociationFrom<N, NamesTuple>;

                // TODO: expand the possibilities of this function... at the moment it only takes in rvalue references to instances...
                template<typename ...Instances>
                static constexpr InstancesTuple createCallBacks(Instances ...instances) noexcept {
                    InstancesTuple temporaryForRvalueRefs = std::forward_as_tuple(instances...);
                    InstancesTuple instancesTuple;
                    index_apply<size>([&](auto... Indices) {
                        return std::make_tuple(
                                static_cast<InstanceTypes>(
                                        std::exchange(
                                                std::get<Indices>(instancesTuple),
                                                std::get<Indices>(temporaryForRvalueRefs)
                                        )
                                )...
                        );
                    });

                    return instancesTuple;
                }

                template<typename Name>
                static constexpr auto getInstanceNamed(InstancesTuple& instancesTuple) noexcept
                -> decltype(std::get<IndexOf<Name, NamesTuple>::value>(std::declval<InstancesTuple&>())) {
                    return std::get<IndexOf<Name, NamesTuple>::value>(instancesTuple);
                }

            };
        };
    };

    /**
     * Since we cannot have *no* typename in CallBackArgument, but some callbacks might not take in any parameters, this helps lookup and definition
     */
    using VoidVoidCallback = std::function<void()>;
    using NoArguments = EmptyDescription;
}

#endif //SENSORGATEWAY_CALLBACKTUPLETYPE_H
