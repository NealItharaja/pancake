#include <string>
#include <vector>
#include <unordered_map>
#include <shared_mutex>
#include <cstdint>

class update_cache {
public:
    update_cache() = default;

    explicit update_cache(size_t initial_replicas);

    bool replica_needs_update(const std::string& key, size_t replica_id) const;

    void mark_update(
        const std::string& key,
        const std::string& value,
        size_t num_replicas
    );

    void clear_replica(const std::string& key, size_t replica_id);

    void resize_replicas(const std::string& key, size_t new_size);

    int choose_replica(
        const std::string& key,
        size_t num_replicas,
        int frequency,
        double p_max
    ) const;

    size_t memory_usage_bytes() const;

private:
    struct entry {
        std::string value;
        std::vector<uint8_t> replica_mask;
    };

    static size_t count_bits(const std::vector<uint8_t>& bits);

    static size_t leading_zeros(const std::vector<uint8_t>& bits);

private:
    mutable std::shared_mutex mutex_;

    std::unordered_map<std::string, entry> table_;
};
