//
// Created by Natsurainko on 2026/5/14.
//

#ifndef RENDERGRAPHTEST_PASSDECLARATION_H
#define RENDERGRAPHTEST_PASSDECLARATION_H

#include <functional>
#include <memory>
#include <string>
#include <typeindex>

#include "Vertix/Mixin/IPassBinding.h"
#include "Vertix/Rendering/RenderResourceView.h"
#include "Vertix/Rendering/Pipeline/RenderPass.h"

namespace Vertix {
    struct PassResourceDeclaration {
        enum class PassResourceOperation {
            READ,
            WRITE
        } operation;

        std::string resourceName;
        std::shared_ptr<IPassBinding> resourceBinding;
        RenderResourceViewDesc viewDesc;
    };

    struct PassDeclaration {
        std::vector<PassResourceDeclaration> Resources;
        std::vector<std::type_index> PassDependencies;
        std::function<std::unique_ptr<RenderPass>()> factory;
        std::type_index passType = typeid(void);
        bool hasSideEffect = false;
    };

    template<RenderPassType TRenderPass>
    class PassDeclarationBuilder {
    public:
        template<RenderPassType TDependency>
        PassDeclarationBuilder& DependsAfter() {
            passDeclaration.PassDependencies.emplace_back(typeid(TDependency));
            return *this;
        }

        template<RenderResourceViewType VType>
        PassDeclarationBuilder& Write(
            const std::string& resourceName,
            const RenderResourceView<VType>* TRenderPass::* field)
        {
            RenderResourceViewDesc viewDesc = {};
            viewDesc.type = VType;

            passDeclaration.Resources.emplace_back(PassResourceDeclaration {
                .operation = PassResourceDeclaration::PassResourceOperation::WRITE,
                .resourceName = resourceName,
                .resourceBinding = std::make_unique<PassBinding<const RenderResourceView<VType>*>>(field),
                .viewDesc = viewDesc
            });

            return *this;
        }

        template<RenderResourceViewType VType>
        PassDeclarationBuilder& Write(
            const std::string& resourceName,
            const RenderResourceView<VType>* TRenderPass::* field,
            const RenderResourceViewDesc &desc)
        {
            RenderResourceViewDesc viewDesc = desc;
            viewDesc.type = VType;

            passDeclaration.Resources.emplace_back(PassResourceDeclaration {
                .operation = PassResourceDeclaration::PassResourceOperation::WRITE,
                .resourceName = resourceName,
                .resourceBinding = std::make_unique<PassBinding<const RenderResourceView<VType>*>>(field),
                .viewDesc = viewDesc
            });

            return *this;
        }

        template<RenderResourceViewType VType>
        PassDeclarationBuilder& Read(
            const std::string& resourceName,
            const RenderResourceView<VType>* TRenderPass::* field)
        {
            RenderResourceViewDesc viewDesc = {};
            viewDesc.type = VType;

            passDeclaration.Resources.emplace_back(PassResourceDeclaration {
                .operation = PassResourceDeclaration::PassResourceOperation::READ,
                .resourceName = resourceName,
                .resourceBinding = std::make_shared<PassBinding<const RenderResourceView<VType>*>>(field),
                .viewDesc = viewDesc
            });

            return *this;
        }

        template<RenderResourceViewType VType>
        PassDeclarationBuilder& Read(
            const std::string& resourceName,
            const RenderResourceView<VType>* TRenderPass::* field,
            const RenderResourceViewDesc &desc)
        {
            RenderResourceViewDesc viewDesc = desc;
            viewDesc.type = VType;

            passDeclaration.Resources.emplace_back(PassResourceDeclaration {
                .operation = PassResourceDeclaration::PassResourceOperation::READ,
                .resourceName = resourceName,
                .resourceBinding = std::make_shared<PassBinding<const RenderResourceView<VType>*>>(field),
                .viewDesc = viewDesc
            });

            return *this;
        }

        PassDeclarationBuilder& SideEffect() {
            passDeclaration.hasSideEffect = true;
            return *this;
        }

        [[nodiscard]]
        PassDeclaration Build() {
            return std::move(passDeclaration);
        }

    private:
        friend class RenderPipelineBuilder;

        PassDeclaration passDeclaration {
            .passType = typeid(TRenderPass),
        };

        template<typename... CtorArgs>
        void Construct(CtorArgs&&... ctorArgs) {
            passDeclaration.factory = [args = std::make_tuple(std::forward<CtorArgs>(ctorArgs)...)]() mutable {
                return std::apply([]<typename... TArg>(TArg&&... a) {
                    return std::make_unique<TRenderPass>(std::forward<TArg>(a)...);
                }, std::move(args));
            };
        }

        template<typename TMember>
        struct PassBinding : IPassBinding {
            explicit PassBinding(TMember TRenderPass::* member) :member(member) {}
            TMember TRenderPass::* member;
            TRenderPass* instance;

            void Target(void *pass) override { instance = static_cast<TRenderPass*>(pass); }
            void Inject(const void* resource) override { instance->*member = static_cast<TMember>(resource); }
        };
    };
}

#endif //RENDERGRAPHTEST_PASSDECLARATION_H
