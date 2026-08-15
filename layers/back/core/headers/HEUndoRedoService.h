#ifndef HE_UNDO_REDO_SERVICE_H
#define HE_UNDO_REDO_SERVICE_H

#include <deque>
#include <memory>
#include <string>
#include <vector>

// Host-independent undo/redo service.
//
// Commands are supplied by the frontend. The service only owns the history
// stacks and calls undo()/redo() in LIFO order.
class HEUndoRedoCommand {
public:
    virtual ~HEUndoRedoCommand() = default;
    virtual std::string name() const = 0;
    virtual bool undo() = 0;
    virtual bool redo() = 0;
};

class HEUndoRedoService {
public:
    using CommandPtr = std::shared_ptr<HEUndoRedoCommand>;

    // Start a multi-command action. Commands recorded while a transaction is
    // open are committed together by commit().
    void begin(const std::string& actionName){
        // Be forgiving with an already-open transaction: flush the previous
        // pending commands as their own action instead of losing them.
        if(inTransaction_){
            if(not pendingCommands_.empty())
                commit();
            else
                inTransaction_ = false;
        }

        inTransaction_ = true;
        pendingName_ = actionName;
        pendingCommands_.clear();
    }

    // Single commands (recorded without begin()) are committed immediately.
    // Commands recorded between begin() and commit() are batched as one action.
    void add(CommandPtr command){
        if(not command)
            return;

        if(not inTransaction_){
            Action action;
            action.name = command->name();
            action.commands.push_back(std::move(command));
            undoStack_.push_back(std::move(action));
            redoStack_.clear();
            return;
        }

        pendingCommands_.push_back(std::move(command));
    }

    bool commit(){
        if(not inTransaction_)
            return false;

        inTransaction_ = false;
        if(pendingCommands_.empty()){
            pendingName_.clear();
            return false;
        }

        undoStack_.push_back({pendingName_, std::move(pendingCommands_)});
        pendingCommands_.clear();
        pendingName_.clear();
        redoStack_.clear();
        return true;
    }

    void cancel(){
        inTransaction_ = false;
        pendingCommands_.clear();
        pendingName_.clear();
    }

    bool undo(){
        // If the caller forgot to commit an open transaction, treat those
        // already-applied commands as the newest action and undo them.
        if(inTransaction_){
            if(not pendingCommands_.empty())
                commit();
            else
                cancel();
        }

        if(undoStack_.empty())
            return false;

        Action action = std::move(undoStack_.back());
        undoStack_.pop_back();

        bool success = true;
        for(auto it = action.commands.rbegin(); it != action.commands.rend(); ++it){
            if(not (*it)->undo())
                success = false;
        }

        redoStack_.push_back(std::move(action));
        return success;
    }

    bool redo(){
        // Uncommitted changes invalidate the redo branch.
        if(inTransaction_){
            if(not pendingCommands_.empty())
                commit();
            else
                cancel();
        }

        if(redoStack_.empty())
            return false;

        Action action = std::move(redoStack_.back());
        redoStack_.pop_back();

        bool success = true;
        for(auto& command : action.commands){
            if(not command->redo())
                success = false;
        }

        undoStack_.push_back(std::move(action));
        return success;
    }

    void clear(){
        inTransaction_ = false;
        pendingCommands_.clear();
        pendingName_.clear();
        undoStack_.clear();
        redoStack_.clear();
    }

    bool canUndo() const{
        return not undoStack_.empty();
    }

    bool canRedo() const{
        return not redoStack_.empty();
    }

    bool isInTransaction() const{
        return inTransaction_;
    }

    std::string undoActionName() const{
        if(undoStack_.empty())
            return {};
        return undoStack_.back().name;
    }

    std::string redoActionName() const{
        if(redoStack_.empty())
            return {};
        return redoStack_.back().name;
    }

    std::size_t undoCount() const{
        return undoStack_.size();
    }

    std::size_t redoCount() const{
        return redoStack_.size();
    }

private:
    struct Action {
        std::string name;
        std::vector<CommandPtr> commands;
    };

    bool inTransaction_ = false;
    std::string pendingName_;
    std::vector<CommandPtr> pendingCommands_;
    std::vector<Action> undoStack_;
    std::vector<Action> redoStack_;
};

#endif // HE_UNDO_REDO_SERVICE_H
