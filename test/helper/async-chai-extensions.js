module.exports = function(chai) {
    chai.assert.throwsAsync = async function(f) {
        try {
            await f();
        } catch(e) {
            return;
        }
        throw new Error('Expected function to throw');
    };
};
