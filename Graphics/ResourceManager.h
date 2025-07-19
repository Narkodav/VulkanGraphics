#pragma once
#include <vector>
#include <functional>
#include <string>
#include <iostream>
#include <unordered_set>

//could probably add resource tracking
#define _USE_REDOURCE_MANAGER_QUEUE_BASED_

#ifdef _USE_REDOURCE_MANAGER_QUEUE_BASED_
namespace Graphics {

    class ResourceManager
    {
    public:
        struct ResourceInfo {
            std::string name;
            std::string type;
            void* handle;
            size_t size;
            std::string creationFile;
            int creationLine;
        };

    private:
        std::vector<std::function<void()>> cleanupStack;
        std::unordered_map<void*, ResourceInfo> activeResources;

    public:

        // Execute all cleanup functions in reverse order
        void cleanup() {
            while (!cleanupStack.empty()) {
                cleanupStack.back()();  // Execute last function
                cleanupStack.pop_back();  // Remove it from stack
            }
        }

        ~ResourceManager() {
            if (!activeResources.empty()) {
                std::cerr << "WARNING: Resource leaks detected!\n";
                printActiveResources();
            }
            cleanup();
        }

        template<typename T>
        void registerResource(T* handle,
            const std::string& name,
            std::function<void()> cleanup,
            const char* file = __FILE__,
            int line = __LINE__,
            size_t size = 0)
        {
            // Track the resource first
            ResourceInfo info{
                name,
                typeid(T).name(),
                static_cast<void*>(handle),
                size,
                file,
                line
            };

            activeResources[static_cast<void*>(handle)] = info;
            cleanupStack.push_back([this, handle, cleanup]() {
                cleanup();  // Run the provided cleanup first
                untrackResource(static_cast<void*>(handle));  // Then remove from tracking
                });
        }

        void untrackResource(void* handle) {
            activeResources.erase(handle);
        }

        // Debug utilities
        void printActiveResources() const {
            std::cout << "Active Resources:\n";
            for (const auto& [handle, info] : activeResources) {
                std::cout << "Resource: " << info.name
                    << "\n  Type: " << info.type
                    << "\n  Handle: " << handle
                    << "\n  Size: " << info.size << " bytes"
                    << "\n  Created at: " << info.creationFile << ":" << info.creationLine
                    << "\n\n";
            }
        }

        size_t getTotalResourceMemory() const {
            size_t total = 0;
            for (const auto& [handle, info] : activeResources) {
                total += info.size;
            }
            return total;
        }

        bool hasResource(void* handle) const {
            return activeResources.contains(handle);
        }

        size_t getResourceCount() const {
            return activeResources.size();
        }
    };


#else

//default tree based resource manager

class ResourceManager {
private:
    struct Resource {
        std::string name;
        void* handle;
        size_t size;
        std::string creationFile;
        int creationLine;

        std::vector<std::string> dependencies; //the ones resource depends on
        std::vector<std::string> dependents;   //the ones that depend on a resource
        std::function<void()> destroyFn;
        std::function<void()> createFn;

        // For cycle detection
        bool visiting = false;
    };

    std::unordered_map<std::string, Resource> resources;
    std::vector<std::string> rootNodes;

public:
    template<typename T>
    void registerResource(T* handle,
        const std::string& name,
        std::function<void()> destroyFn,
        const std::vector<std::string>& dependencies = {},
        const char* file = __FILE__,
        int line = __LINE__,
        size_t size = 0,
        std::function<void()> createFn = nullptr) {

        Resource resource;
        resource.name = name;
        resource.handle = static_cast<void*>(handle);
        resource.size = size;
        resource.creationFile = file;
        resource.creationLine = line;
        resource.destroyFn = std::move(destroyFn);
        resource.createFn = std::move(createFn);

        // Check for self-dependency
        if (std::find(dependencies.begin(), dependencies.end(), name)
            != dependencies.end()) {
            throw std::runtime_error(
                "Resource '" + name + "' cannot depend on itself");
        }

        if (std::find(dependencies.begin(), dependencies.end(), name)
            != dependencies.end()) {
            throw std::runtime_error(
                "Resource '" + name + "' cannot depend on itself");
        }

        // Setup dependencies
        for (const auto& depName : dependencies) {
            auto it = resources.find(depName);
            if (it != resources.end()) {
                resource.dependencies.push_back(it->second.name);
                it->second.dependents.push_back(resource.name);
            }
        }

        if (resource.dependencies.empty()) {
            rootNodes.push_back(name);
        }

        resources.emplace(name, std::move(resource));

        //// Check for cycles after adding the new resource
        //std::unordered_set<std::string> visited;
        //for (auto& [_, res] : resources) {
        //    res.visiting = false;
        //}

        //if (hasCycle(resources[name], visited)) {
        //    // Remove the resource we just added
        //    resources.erase(name);
        //    throw std::runtime_error(
        //        "Adding resource '" + name + "' would create a circular dependency");
        //}
    }

    void destroyResourceTree(const std::string& rootName) {
        if (auto it = resources.find(rootName); it != resources.end()) {
            destroyResourceRecursive(it->second);
        }
    }

    // Debug utility to print dependency graph
    void printDependencyGraph() const {
        for (const auto& [name, resource] : resources) {
            std::cout << name << " depends on: ";
            for (const auto& dep : resource.dependencies) {
                std::cout << dep << " ";
            }
            std::cout << "\n";
        }
    }

    void cleanup()
    {
        for (auto& root : rootNodes)
            destroyResourceTree(root);
        rootNodes.clear();
    }

private:
    void destroyResourceRecursive(Resource& resource) {
        for (auto& dependent : resource.dependents) {
            auto it = resources.find(dependent);
            if (it != resources.end()) {
                destroyResourceRecursive(it->second);
            }
            else throw std::runtime_error(
                "Resource '" + dependent + "' not found");
        }

        for (auto& dependency : resource.dependencies) {
            auto it = resources.find(dependency);
            if (it != resources.end()) {
                it->second.dependents.erase(
                    std::remove(it->second.dependents.begin(),
                        it->second.dependents.end(),
                        resource.name));
            }
        }

        if (resource.destroyFn) {
            resource.destroyFn();
        }

        resources.erase(resource.name);
    }

    //// Detect cycles using DFS
    //bool hasCycle(Resource& resource, std::unordered_set<std::string>& visited) {
    //    if (resource.visiting) {
    //        return true; // Found a cycle
    //    }

    //    if (visited.contains(resource.name)) {
    //        return false; // Already checked this path
    //    }

    //    resource.visiting = true;
    //    visited.insert(resource.name);

    //    for (auto& dep : resource.dependencies) {
    //        if (hasCycle(*dep, visited)) {
    //            // Help with debugging by printing the cycle
    //            std::cerr << "Cycle detected: " << resource.name
    //                << " -> " << dep->name << "\n";
    //            return true;
    //        }
    //    }

    //    resource.visiting = false;
    //    return false;
    //}
};

#endif
}