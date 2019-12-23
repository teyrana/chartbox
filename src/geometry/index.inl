// GPL v3 (c) 2020

namespace yggdrasil::geometry {

// ========= ========= ========= Row Major Index ========= ========= =========
template<size_t dimension, typename cell_t, typename storage_t>
RowMajorIndex<dimension,cell_t,storage_t>::RowMajorIndex( storage_t& _data):
    data(_data)
{}

template<size_t dimension, typename cell_t, typename storage_t>
constexpr cell_t& RowMajorIndex<dimension,cell_t,storage_t>::lookup( const uint32_t i, const uint32_t j) {
    return this->operator()(i,j);
}

template<size_t dimension, typename cell_t, typename storage_t>
constexpr cell_t& RowMajorIndex<dimension,cell_t,storage_t>::operator()( const uint32_t i, const uint32_t j) {
    return data[ i + ( j * dimension) ];
}

template<size_t dimension, typename cell_t, typename storage_t>
constexpr const cell_t& RowMajorIndex<dimension,cell_t,storage_t>::operator()( const uint32_t i, const uint32_t j) const {
    return data[ i + ( j * dimension) ];
}

// ========= ========= ========= Free Functions: Z-Order Curve Index========= ========= =========

inline constexpr uint8_t _calculate_padding(const size_t dimension){
    size_t power = 1;
    size_t padding = 64;

    // this loop basically calculates log2(dimension), but in constexpr 
    while( dimension > power){
        power <<= 1;
        padding -= 2;
    }

    return padding;
}

inline uint64_t _interleave( const uint32_t input) {
    uint64_t word = input;
    word = (word ^ (word<<16)) & 0x0000ffff0000ffff;
    word = (word ^ (word<<8))  & 0x00ff00ff00ff00ff;
    word = (word ^ (word<<4))  & 0x0f0f0f0f0f0f0f0f;
    word = (word ^ (word<<2))  & 0x3333333333333333; 
    word = (word ^ (word<<1))  & 0x5555555555555555; 
    return word;
}

// ========= ========= ========= Z-Order Curve Index========= ========= =========
template<size_t dimension, typename cell_t, typename storage_t>
ZOrderCurveIndex<dimension,cell_t,storage_t>::ZOrderCurveIndex( storage_t& _data):
    data(_data)
{}

template<size_t dimension, typename cell_t, typename storage_t>
constexpr cell_t ZOrderCurveIndex<dimension, cell_t,storage_t>::lookup( const uint32_t i, const uint32_t j){
    return this->operator()(i,j);
}

template<size_t dimension, typename cell_t, typename storage_t>
constexpr cell_t& ZOrderCurveIndex<dimension, cell_t,storage_t>::operator()( const uint32_t i, const uint32_t j){
    const uint64_t index = _interleave(i) + (_interleave(j) << 1);
 
    // const uint64_t raw_index = interleave(i) + (interleave(j) << 1);
    // const uint64_t index = raw_index << ZOrderCurveIndex<dimension, cell_t>::padding;
    
    return data[index];
}

template<size_t dimension, typename cell_t, typename storage_t>
constexpr const cell_t& ZOrderCurveIndex<dimension, cell_t,storage_t>::operator()( const uint32_t i, const uint32_t j) const {

    const uint64_t index = _interleave(i) + (_interleave(j) << 1);
 
    // const uint64_t raw_index = interleave(i) + (interleave(j) << 1);
    // const uint64_t index = raw_index << ZOrderCurveIndex<dimension, cell_t>::padding;
    
    return data[index];
}


} // namespace yggdrasil::geometry