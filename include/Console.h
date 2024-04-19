#pragma once

//Credit: Based on ConsoleUtilSS-NG by VersuchDrei
class Console
{
public:
    static inline void ExecuteCommand(std::string a_command)
    {
        const auto scriptFactory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::Script>();
        const auto script        = scriptFactory ? scriptFactory->Create() : nullptr;
        if (script) {
            const auto selectedRef = GetSelectedRef();
            script->SetCommand(a_command);
            CompileAndRun(script, selectedRef.get());
            delete script;
        }
    }

    inline static void CompileAndRun(RE::Script* script, RE::TESObjectREFR* targetRef, RE::COMPILER_NAME name = RE::COMPILER_NAME::kSystemWindowCompiler)
    {
        RE::ScriptCompiler compiler;
        CompileAndRunImpl(script, &compiler, name, targetRef);
    }

    inline static void CompileAndRunImpl(RE::Script* script, RE::ScriptCompiler* compiler, RE::COMPILER_NAME name, RE::TESObjectREFR* targetRef)
    {
        using func_t = decltype(CompileAndRunImpl);
        REL::Relocation<func_t> func{ RELOCATION_ID(21416, REL::Module::get().version().patch() < 1130 ? 21890 : 441582) };
        return func(script, compiler, name, targetRef);
    }

    inline static RE::NiPointer<RE::TESObjectREFR> GetSelectedRef()
    {
        REL::Relocation<RE::ObjectRefHandle*> selectedRef{ RELOCATION_ID(519394, REL::Module::get().version().patch() < 1130 ? 405935 : 504099) };
        auto handle = *selectedRef;
        return handle.get();
    }
};
