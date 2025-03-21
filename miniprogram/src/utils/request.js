/**
 * 封装微信小程序请求方法
 * 返回Promise，支持请求拦截和响应拦截
 */

const config = require('../../config.js');

// 基础URL
const baseURL = `http://${config.serverIp}:${config.serverPort}`;

/**
 * 封装的请求方法
 * @param {string} url - 请求路径
 * @param {string} method - 请求方法 GET/POST/PUT/DELETE
 * @param {Object} data - 请求参数
 * @param {Object} options - 其他选项，如header等
 * @returns {Promise} Promise对象
 */
const request = (url, method = 'GET', data = {}, options = {}) => {
  // 完整URL
  const fullUrl = `${baseURL}${url}`;
  
  // 默认header
  const header = {
    'content-type': 'application/json',
    ...options.header
  };

  // 显示加载中
  if (options.showLoading !== false) {
    wx.showLoading({
      title: options.loadingText || '加载中...',
      mask: true
    });
  }

  // 返回Promise
  return new Promise((resolve, reject) => {
    wx.request({
      url: fullUrl,
      method,
      data,
      header,
      timeout: config.requestTimeout || 10000,
      success: (res) => {
        // 隐藏加载提示
        if (options.showLoading !== false) {
          wx.hideLoading();
        }

        // 判断请求是否成功
        if (res.statusCode >= 200 && res.statusCode < 300) {
          if (config.enableDebugLog) {
            console.log(`[Request Success] ${method} ${url}`, res.data);
          }
          resolve(res.data);
        } else {
          console.error(`[Request Error] ${method} ${url}`, res);
          // 处理错误响应
          handleError(res.statusCode, reject);
        }
      },
      fail: (err) => {
        // 隐藏加载提示
        if (options.showLoading !== false) {
          wx.hideLoading();
        }
        
        console.error(`[Request Failed] ${method} ${url}`, err);
        // 处理网络错误
        reject({
          code: -1,
          message: '网络请求失败，请检查网络连接'
        });
      }
    });
  });
};

/**
 * 处理请求错误
 * @param {number} statusCode - HTTP状态码
 * @param {Function} reject - Promise的reject函数
 */
const handleError = (statusCode, reject) => {
  let message = '请求失败';
  
  switch (statusCode) {
    case 400:
      message = '请求参数错误';
      break;
    case 401:
      message = '未授权，请重新登录';
      // 可以在这里处理登录失效的逻辑
      break;
    case 403:
      message = '拒绝访问';
      break;
    case 404:
      message = '请求的资源不存在';
      break;
    case 500:
      message = '服务器内部错误';
      break;
    default:
      message = `请求失败，状态码：${statusCode}`;
  }

  wx.showToast({
    title: message,
    icon: 'none',
    duration: 2000
  });

  reject({
    code: statusCode,
    message
  });
};

/**
 * 封装GET请求
 * @param {string} url - 请求路径
 * @param {Object} data - 请求参数
 * @param {Object} options - 其他选项
 * @returns {Promise} Promise对象
 */
const get = (url, data = {}, options = {}) => {
  return request(url, 'GET', data, options);
};

/**
 * 封装POST请求
 * @param {string} url - 请求路径
 * @param {Object} data - 请求参数
 * @param {Object} options - 其他选项
 * @returns {Promise} Promise对象
 */
const post = (url, data = {}, options = {}) => {
  return request(url, 'POST', data, options);
};

/**
 * 封装PUT请求
 * @param {string} url - 请求路径
 * @param {Object} data - 请求参数
 * @param {Object} options - 其他选项
 * @returns {Promise} Promise对象
 */
const put = (url, data = {}, options = {}) => {
  return request(url, 'PUT', data, options);
};

/**
 * 封装DELETE请求
 * @param {string} url - 请求路径
 * @param {Object} data - 请求参数
 * @param {Object} options - 其他选项
 * @returns {Promise} Promise对象
 */
const del = (url, data = {}, options = {}) => {
  return request(url, 'DELETE', data, options);
};

module.exports = {
  request,
  get,
  post,
  put,
  del
}; 