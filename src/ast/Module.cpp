#include <wasm/wasm.hpp>

#include <iterator>
#include <iostream>
#include <sstream>

WASM_NAMESPACE_BEGIN

Error<std::shared_ptr<execute::Store>> Module::instantiate()
{
    using RetType = Error<std::shared_ptr<execute::Store>>;

    /* this method follows https://webassembly.github.io/spec/core/exec/modules.html#instantiation */

    /* 1. validate module */
    {
        auto is_valid = validate();
        if (!is_valid)
        {
            return RetType(std::unexpected(is_valid.error()));
        }
    }

    auto s = std::make_shared<execute::Store>();
    auto moduleinstance = std::make_shared<execute::ModuleInstance>();
    s->replaceModuleInstance(std::move(moduleinstance));

    /* 2., 3. only semantics */
    /* 4. check externaddr
       TODO: add function param vec<ExternAddr> and check length with imports */
    std::cout << "[WARNING] Imports not supported yet!\n";

    /* 5. check if all extern addresses match import types */
    /* 6., 7., 8., 9. only semantics */
    /* 10. go through tables, collect initialization expressions */
    std::cout << "[WARNING] Tables not supported yet!\n";

    /* 11.-13. collect globals,
        => they are already collected in the `globals` member variable */

    /* 14., 15. TODO: elements */
    std::cout << "[WARNING] Elements not supported yet!\n";
    /* 16.-19. evaluate initializers */
    execute::State z(s.get());
    for (const auto &global : globals)
    {
        auto exe_success = global.getExpr().execute(z);
        if (!exe_success)
        {
            std::stringstream msg;
            msg << "Module::instantiate(): error while executing initialization code for global\n"
                << "  Reason: " << exe_success.error();
            return RetType(std::unexpected(msg.str()));
        }

        auto value = z.popOperand();
        if (!value)
        {
            std::stringstream msg;
            msg << "Module::instantiate(): initializer expression for global left operand stack empty"
                << "  " << value.error();
            return RetType(std::unexpected(msg.str()));
        }

        if (value.value().getType() != global.getGlobalType().reftype)
        {
            std::stringstream msg;
            msg << "Module::instantiate(): result of global initializer does not match declared type\n"
                << "  Expected: " << global.getGlobalType().reftype << ", actual: " << value.value().getType() << "\n";
            return RetType(std::unexpected(msg.str()));
        }

        execute::GlobalInstance gi(global.getGlobalType(), value.value());
        moduleinstance->addGlobalAddr(s->allocateGlobal(std::move(gi)));
    }

    /* 20.-24. evaluate table and element initializers */
    std::cout << "[WARNING] Table and Elements not supported yet!\n";

    /* 25. store computed values in module instance
        => Done partially at end of loop in 16.-19. */

    std::cout << "[WARNING] TODO: when implementing importing, offset function addresses in moduleinstance\n";
    for (const auto &f : functions)
    {
        s->addFunctionInstance(execute::FunctionInstance(types[f.getType()],
                                                         moduleinstance, f.getCode()));
    }

    for (const auto &m : memory_types)
    {
        s->addMemoryInstance(execute::MemoryInstance(m));
    }

    for (const auto &e : exports)
    {
        execute::ExportInstance e_inst(e);
        if (options::is_verbose)
        {
            std::stringstream msg;
            msg << "Module exports \"" << getName(e.name) << "\" with index " << e_inst.getAddress() << "\n";
        }
        moduleinstance->addExport(std::move(e_inst));
    }

    /* 26.-31. TODO: evaluate start etc. */

    /* 32. return moduleinstance */
    return RetType(std::move(s));
}

Error<void> Module::validateSingleType(RecType &rectype)
{
    const std::size_t x = defined_types.size();

    auto roll_success = rectype.roll(x);
    if (!roll_success)
    {
        std::stringstream msg;
        msg << "Module::validateSingleType(RecType&): Can't roll type\n"
            << "  Reason: " << roll_success.error();
        return Error<void>(std::unexpected(msg.str()));
    }

    defined_types.insert(defined_types.end(),
                         std::make_move_iterator(roll_success.value().begin()),
                         std::make_move_iterator(roll_success.value().end()));

    auto rectype_valid = rectype.validate(*this, x);
    if (!rectype_valid)
    {
        std::stringstream msg;
        msg << "Module::validateSingleType(RecType&): Type is invalid\n"
            << "  Reason: " << rectype_valid.error();
        return Error<void>(std::unexpected(msg.str()));
    }

    return Error<void>();
}

Error<void> Module::validateTypes()
{
    if (options::is_verbose)
        std::cout << "Module::validateTypes(): Validating types\n";

    const std::size_t num_types = types.size();
    for (uint32_t i = 0; i < num_types; ++i)
    {
        auto type_success = validateSingleType(types[i]);
        if (!type_success)
        {
            std::stringstream msg;
            msg << "Module::validateTypes(): Can't validate type with index " << i << "\n"
                << "  Reason: " << type_success.error();
            return Error<void>(std::unexpected(msg.str()));
        }
    }

    return Error<void>();
}

Error<void> Module::validate()
{
    if (options::is_verbose)
        std::cout << "Module::validate(): Starting validation\n";

    auto types_success = validateTypes();
    if (!types_success)
    {
        std::stringstream msg;
        msg << "Module::validate(): Can't validate types\n"
            << "  Reason: " << types_success.error();
        return Error<void>(std::unexpected(msg.str()));
    }

    std::cout << "[WARNING] Globals, memories and code is not typechecked!\n";

    return Error<void>();
};

WASM_NAMESPACE_END
