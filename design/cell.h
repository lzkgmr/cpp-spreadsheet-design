#pragma once

#include "common.h"
#include "formula.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>
#include <algorithm>
#include <optional>
#include <unordered_set>

class Cell : public CellInterface {
public:
    explicit Cell(SheetInterface& sheet);

    void Set(const std::string& text);

    void Clear();

    Value GetValue() const override;

    std::string GetText() const override;
    
    std::vector<Position> GetReferencedCells() const override;

private:
    class Impl {
    public:
        virtual ~Impl() = default;

        virtual Value GetValue() const = 0;

        virtual std::string GetText() const = 0;

        virtual std::optional<FormulaInterface::Value> GetCache() const;

        virtual void ResetCache();
        
        virtual std::vector<Position> GetReferencedCells() const;
    };

    class EmptyImpl final : public Impl {
    public:
        Value GetValue() const override;

        std::string GetText() const override;
    };

    class TextImpl final : public Impl {
    public:
        explicit TextImpl(std::string expression);

        Value GetValue() const override;

        std::string GetText() const override;

    private:
        std::string value_;
    };

    class FormulaImpl final : public Impl {
    public:
        explicit FormulaImpl(std::string expression,
                            const SheetInterface& sheet);

        Value GetValue() const override;

        std::string GetText() const override;

        void ResetCache();

        std::optional<FormulaInterface::Value> GetCache() const;
        
        std::vector<Position> GetReferencedCells() const override;

    private:
        std::unique_ptr<FormulaInterface> formula_;
        mutable std::optional<FormulaInterface::Value> cache_;
        const SheetInterface& sheet_;
    };

    void InvalidateCache();

    void InvalidateCacheInner(std::unordered_set<Cell *> &visited_cells);
    
    void UpdateDependencies(const std::vector<Position> &new_referenced_cells);
        
    void DetectCircularDependency(const std::vector<Position> &ref_cells) const;

    void DetectInnerCircularDependency(const std::vector<Position> &ref_cells,
                                        std::unordered_set<CellInterface *> &visited_cells) const;


    SheetInterface& sheet_;
    std::unique_ptr<Impl> impl_;
    std::unordered_set<Cell *> referenced_cells_;
    std::unordered_set<Cell *> dependent_cells_;
};
